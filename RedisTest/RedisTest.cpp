// RedisTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <boost/redis/connection.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/describe.hpp>
#include <boost/asio/consign.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <string>
#include <map>

#if defined(BOOST_ASIO_HAS_CO_AWAIT)
#define BOOST_REDIS_SEPARATE_COMPILATION
#include <boost/redis/src.hpp>

#include <boost/json/serialize.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/value_from.hpp>
#include <boost/json/value_to.hpp>
#include <boost/redis/resp3/serialization.hpp>
#include <boost/redis.hpp>

namespace asio = boost::asio;
namespace resp3 = boost::redis::resp3;
using namespace boost::describe;
using boost::redis::request;
using boost::redis::response;
using boost::redis::ignore_t;
using boost::redis::config;
using boost::redis::connection;
using boost::redis::resp3::node_view;

// Struct that will be stored in Redis using json serialization. 
struct user {
    std::string name;
    std::string age;
    std::string country;
};

// The type must be described for serialization to work.
BOOST_DESCRIBE_STRUCT(user, (), (name, age, country))

// Boost.Redis customization points (example/json.hpp)
void boost_redis_to_bulk(std::string& to, user const& u)
{
    to = boost::json::serialize(boost::json::value_from(u));
}

void
boost_redis_from_bulk(
    user& u,
    node_view const& node,
    boost::system::error_code&)
{
    u = boost::json::value_to<user>(boost::json::parse(node.value));
}

#include "../Network/BaseCoroutine.h"
Awaitable<int> TestCoro()
{
    std::cout << ":test" << std::endl;
    co_await std::suspend_always{};
    std::cout << ":test2" << std::endl;
    co_return 1;
}

auto co_main(config cfg) -> asio::awaitable<void>
{
    auto ex = co_await asio::this_coro::executor;
    auto conn = std::make_shared<connection>(ex);
    conn->async_run(cfg, {}, asio::consign(asio::detached, conn));

    // user object that will be stored in Redis in json format.
    user const u{ "Joao", "58", "Brazil" };

    // Stores and retrieves in the same request.
    request req;
    std::string use;
    boost_redis_to_bulk(use, u);

    req.push("SET", "json-key", use); // Stores in Redis.
    req.push("GET", "json-key"); // Retrieves from Redis.
    req.push("ZADD", "test", "2", "test1", "3", "test2");
    std::map<std::string, int> test = { {"test1", 2}, {"test2", 3} };
    std::vector<std::string> insert;
    for (auto& [key, value] : test)
    {
        insert.push_back(std::to_string(value));
        insert.push_back(key);
    }

    req.push_range("ZADD", "test2", insert.begin(), insert.end());
    boost::redis::response<ignore_t, user, int, int> resp;
    co_await conn->async_exec(req, resp);

    conn->cancel();
}

#endif // defined(BOOST_ASIO_HAS_CO_AWAIT)

#include "RedisCommand.h"
using boost::asio::co_spawn;
using boost::asio::detached;
int main()
{
    asio::io_context io;

    user u{ "Joao", "58", "Brazil" };
    /*auto set = SetCommand("test", u);
    auto get = GetCommand<user>("test");
    RedisCommands cmd(set, get);*/

    boost::redis::config config;
    config.addr = boost::redis::address{ "127.0.0.1", "6379" };

    // co_main을 co_spawn으로 실행
    boost::asio::co_spawn(io, co_main(config), detached);

    // io_context 실행
    io.run();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
