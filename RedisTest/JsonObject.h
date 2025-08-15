#pragma once

#include <boost/json/serialize.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/value_from.hpp>
#include <boost/json/value_to.hpp>

//// Boost.Redis customization points (example/json.hpp)
//template<typename T>
//void boost_redis_to_bulk(std::string& to, T const& u)
//{
//    to = boost::json::serialize(boost::json::value_from(u));
//}
//
//template<typename T>
//void boost_redis_from_bulk(T& u, node_view const& node, boost::system::error_code&)
//{
//    u = boost::json::value_to<T>(boost::json::parse(node.value));
//}

template<typename T>
std::string ObjectToJson(T const& u)
{
    return boost::json::serialize(boost::json::value_from(u));
}