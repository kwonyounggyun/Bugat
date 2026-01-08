#pragma once
#include <concepts>
#include "BoostAsio.h"
#include "BoostRedis.h"
#include "Log.h"

namespace bugat
{
    template <typename... Args, std::size_t... Is>
    std::tuple<Args...> convert_response_impl(const RedisResponse<Args...>& resp, std::index_sequence<Is...>) {
        return { std::get<Is>(resp).value()... };
    }

    template <typename... Args>
    std::tuple<Args...> convert_response(const RedisResponse<Args...>& resp) {
        return convert_response_impl(resp, std::index_sequence_for<Args...>{});
    }

    class RedisClient;

    template<typename ...CMDs>
    class RedisPipeline
    {
    public:
        using ResponseType = RedisResponse<typename std::decay_t<CMDs>::resType...>;
        using ResultType = std::tuple<typename std::decay_t<CMDs>::resType...>;
        using HandleType = std::function<void(RedisErrorCode, const ResultType&)>;
    
        RedisPipeline(CMDs&& ...cmds) {
            (cmds.Command(_req), ...);
        }
        ~RedisPipeline() {}

        RedisRequest& GetReq() { return _req; }
        ResponseType& GetResp() { return _resp; }

    private:
        RedisRequest _req;
        ResponseType _resp;
    };

    template<typename ...CMDs>
    std::shared_ptr<RedisPipeline<std::decay_t<CMDs>...>> CreateRedisPipeline(CMDs&&... cmds)
    {
        auto pipeline = new RedisPipeline<std::decay_t<CMDs>...>(std::forward<CMDs>(cmds)...);
        return std::shared_ptr<RedisPipeline<std::decay_t<CMDs>...>>(pipeline);
    }

    class RedisConnection
    {
    public:
        RedisConnection() : _stop(false) {}
        ~RedisConnection() {}

        void Stop()
        {
            _stop.store(true, std::memory_order_release);

            if (_conn)
            {
                _conn->cancel();
                _conn = nullptr;
            }
        }

        inline bool IsStop()
        {
            return _stop.load(std::memory_order_acquire);
        }

        void SetConnection(std::shared_ptr<BoostRedisConnection>& conn)
        {
            _conn = conn;
        }

        std::shared_ptr<BoostRedisConnection> GetConnection() { return _conn; }

    private:
        std::shared_ptr<BoostRedisConnection> _conn;
        std::atomic_bool _stop;
    };

    boost::asio::awaitable<void> AwaitConnect(std::shared_ptr<RedisConnection> conn, boost::redis::config config)
    {
        if (conn == nullptr)
            co_return;

        boost::redis::logger logger(boost::redis::logger::level::disabled);
        //boost::redis::logger logger(boost::redis::logger::level::info);
        //boost::redis::logger logger(boost::redis::logger::level::debug);
        while (true)
        {
            // Connection은 연결 시도시마다 새로 생성
            auto newCon = std::make_shared<BoostRedisConnection>(co_await boost::asio::this_coro::executor);
            conn->SetConnection(newCon);
            try
            {
                InfoLog("[Redis] Connecting...");
                co_await newCon->async_run(config, logger, boost::asio::use_awaitable);

                // 여기까지 왔다는 건, async_run이 종료되었다는 뜻.
                // (사용자가 cancel()을 호출했거나, 에러가 나서 끊긴 경우)
            }
            catch (const boost::system::system_error& e)
            {
                ErrorLog("[Redis] Boost Error {}", e.what());
            }
            catch (const std::exception& e)
            {
                ErrorLog("[Redis] Exception: {}", e.what());
            }

            if (conn->IsStop())
                co_return;

            // 이전 커넥션 종료
            newCon->cancel();

            InfoLog("[Redis] Reconnecting in 1s...");

            boost::asio::steady_timer timer(co_await boost::asio::this_coro::executor);
            timer.expires_after(std::chrono::seconds(1));
            co_await timer.async_wait(boost::asio::use_awaitable);
        }
    }

    class RedisClient
    {
    public:
        RedisClient(Executor executor) : _strand(boost::asio::make_strand(executor)), _conn(std::make_shared<RedisConnection>())
        {
        }
        ~RedisClient() 
        {
            Stop();
        }

        void Stop()
        {
            boost::asio::post(_strand, [conn = _conn]() {
                conn->Stop();
                });
        }

        void Connect(std::string host, int port)
        {
            boost::redis::config config;
            config.addr = boost::redis::address{ host, std::to_string(port) };
            config.reconnect_wait_interval = std::chrono::seconds(0);

            boost::asio::co_spawn(_strand, AwaitConnect(_conn, config), boost::asio::detached);
        }

        template<typename Pipeline>
        void Execute(std::shared_ptr<Pipeline>& pipe, Pipeline::HandleType&& handle)
        {
            boost::asio::post(_strand, [conn = _conn, pipe, handle]() {
                if (auto realConn = conn->GetConnection(); realConn)
                {
                    realConn->async_exec(pipe->GetReq(), pipe->GetResp(), [pipe, handle](RedisError ec, size_t byteTrans) {
                        if (ec)
                        {
                            typename Pipeline::ResultType result;
                            handle(RedisErrorCode::Fail, result);
                            return;
                        }

                        typename Pipeline::ResultType result = convert_response(pipe->GetResp());
                        handle(RedisErrorCode::Success, result);
                        });
                }
                else
                {
                    typename Pipeline::ResultType result;
                    handle(RedisErrorCode::Fail, result);
                }
                });
        }

    private:
        std::shared_ptr<RedisConnection> _conn;
        Strand _strand;
    };
}

