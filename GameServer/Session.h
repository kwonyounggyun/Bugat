#pragma once
#include "../Network/Connection.h"
#include "../Core/SerializeObject.h"

namespace bugat
{
	class Session : public bugat::core::SerializeObject<Session>
	{
	public:
		Session() {};
		virtual ~Session() {}

		void Send(int size, char* buf);
		void SetConnection(std::shared_ptr<net::Connection>& conn) { _connection = conn; }

		void Close();

	private:
		std::shared_ptr<net::Connection> _connection;
	};
}

