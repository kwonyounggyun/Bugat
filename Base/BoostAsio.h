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

	using UDPResolver = boost::asio::ip::basic_resolver<boost::asio::ip::udp, Executor>;
	using UDPSocket = boost::asio::basic_datagram_socket<boost::asio::ip::udp, Executor>;
	using UDPEndPoint = boost::asio::ip::basic_endpoint<boost::asio::ip::udp>;

	using Strand = boost::asio::strand<Executor>;

	struct TCPSocket
	{
	public:
		explicit TCPSocket(const Executor& ex) : _socket(ex) {}
		~TCPSocket() {}
		void Close()
		{
			boost::system::error_code ec;
			_socket.close(ec);
			if (ec)
				throw ec;
		}

		void AsyncAccept(Acceptor& acceptor, std::function<void(const BoostError& error)>&& callback)
		{
			acceptor.async_accept(_socket, callback);
		}

		void AsyncConnect(Resolver::results_type& endpoints, std::function<void(const BoostError& error, const EndPoint&)>&& callback)
		{
			boost::asio::async_connect(_socket, endpoints, std::move(callback));
		}

		void AsyncReadSome(char* buf, int size, std::function<void(const BoostError& error, size_t transper)>&& callback)
		{
			_socket.async_read_some(boost::asio::buffer(buf, size), callback);
		}

		void AsyncWriteSome(const std::list<boost::asio::const_buffer>& buf, std::function<void(const BoostError& error, size_t transper)>&& callback)
		{
			_socket.async_write_some(buf, callback);
		}

	private:
		Socket _socket;
	};
}