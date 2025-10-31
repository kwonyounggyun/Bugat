#pragma once
#include <memory>
#include <unordered_map>
#include "../Network/Header.h"

namespace bugat
{
	namespace net
	{
		struct Header;
	}

	class GameSession;
	class GameHandler
	{
	public:
		static void Handle(std::shared_ptr<Session>& session, const net::Header& header, const std::vector<char>& msg);

	private:
		static std::unordered_map<int, BaseHandle*> _handles;
	};

	class BaseHandle
	{
		BaseHandle() {}
		~BaseHandle() {}

		void Handle(std::shared_ptr<Session>& session, const net::Header& header, const std::vector<char>& msg);

	};
}