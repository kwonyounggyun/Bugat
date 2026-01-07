#pragma once
#include <concepts>
#include "BoostAsio.h"
#include "BoostRedis.h"
#include "Log.h"

namespace bugat
{
    //// 헬퍼: Redis response(tuple-like)에서 실제 value들만 뽑아 핸들러를 호출하는 함수
    //template <typename... Args, std::size_t... Is>
    //void call_handler_impl(const std::function<void(RedisError&, Args&...)>& handler,
    //    RedisError& error,
    //    RedisResponse<Args...>& resp,
    //    std::index_sequence<Is...>) {
    //    // std::get<Is>(resp).value()를 통해 실제 데이터만 추출하여 전달
    //    handler(error, std::get<Is>(resp).value()...);
    //}

    //template <typename... Args>
    //void call_handler(const std::function<void(RedisError&, Args&...)>& handler, RedisError& error, RedisResponse<Args...>& resp) {
    //    call_handler_impl(handler, error, resp, std::index_sequence_for<Args...>{});
    //}


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
        friend class RedisClient;

        using ResponseType = RedisResponse<typename std::decay_t<CMDs>::resType...>;
        using ResultType = std::tuple<typename std::decay_t<CMDs>::resType...>;
        using HandleType = std::function<void(RedisError&, const ResultType&)>;
        
    public:
        RedisPipeline(CMDs&& ...cmds) {
            (cmds.Command(_req), ...);
        }
        ~RedisPipeline() {}

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


    template<typename T>
    concept IsCMDType = requires{ 
        typename T::resType; 
        { std::declval<T>().Command(std::declval<RedisRequest&>()) } -> std::same_as<void>;
    };

    template<typename T>
    concept IsPipelineType = requires{
        typename T::HandleType;
        typename T::ResponseType;
    };

    class RedisClient
    {
    public:
        RedisClient(Executor executor) : _strand(boost::asio::make_strand(executor)), _conn(std::make_shared<RedisConnection>(executor)) {}
        ~RedisClient() 
        {
            if (_conn)
            {
                _conn->cancel();
                _conn = nullptr;
            }
        }

        void Connect(std::string host, int port)
        {
            boost::redis::config config;
            config.addr = boost::redis::address{ host, std::to_string(port) };
            config.reconnect_wait_interval = std::chrono::seconds(0);

            boost::redis::logger logger(boost::redis::logger::level::disabled);
            //boost::redis::logger logger(boost::redis::logger::level::info);
            //boost::redis::logger logger(boost::redis::logger::level::debug);
            _conn->async_run(config, logger, [conn = _conn, config](RedisError ec) {
                ErrorLog("Redis Client disconnected. error_code[{}] reason[{}]", ec.value(), ec.what());
                });
            InfoLog("Redis Client connected.");
        }

        template<typename Pipeline>
        void Execute(std::shared_ptr<Pipeline>& pipe, Pipeline::HandleType&& handle)
        {
            boost::asio::post(_strand, [conn = _conn, pipe, handle]() {
                conn->async_exec(pipe->_req, pipe->_resp, [pipe, handle](RedisError ec, size_t byteTrans) {
                    typename Pipeline::ResultType result = convert_response(pipe->_resp);
                    handle(ec, result);
                    });
                });
        }

    private:
        std::shared_ptr<RedisConnection> _conn;
        Strand _strand;
    };
}

