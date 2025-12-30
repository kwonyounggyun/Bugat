#pragma once
#include <boost/asio.hpp>

namespace bugat
{
	using BoostContext = boost::asio::io_context;

	using BoostError = boost::system::error_code;
	using TCP = boost::asio::ip::tcp;

	struct Executor {
		BoostContext& _ioCtx;

		//BoostContext& context() const noexcept { return _ioCtx; }
		BoostContext& query(boost::asio::execution::context_t) const noexcept {
			return _ioCtx;
		}

		Executor require(boost::asio::execution::blocking_t::never_t) const noexcept {
			return *this;
		}

		boost::asio::execution::blocking_t query(boost::asio::execution::blocking_t) const noexcept {
			return boost::asio::execution::blocking_t::never;
		}

		template <typename F>
		void execute(F&& f) const {
			boost::asio::post(_ioCtx, std::forward<F>(f));
		}

		bool operator==(const Executor& other) const noexcept {
			return &_ioCtx == &other._ioCtx;
		}
		bool operator!=(const Executor& other) const noexcept {
			return !(*this == other);
		}
	};

	using Acceptor = boost::asio::basic_socket_acceptor<boost::asio::ip::tcp, Executor>;
	using Socket = boost::asio::basic_stream_socket<boost::asio::ip::tcp, Executor>;
	using Resolver = boost::asio::ip::basic_resolver<boost::asio::ip::tcp, Executor>;
	using EndPoint = boost::asio::ip::basic_endpoint<boost::asio::ip::tcp>;
}