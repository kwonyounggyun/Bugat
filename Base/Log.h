#pragma once
#include "DateTime.h"

#include <mutex>
#include <print>
#define MAX_LOG_LENGTH 1024

#define CriticalLog(data, ...) CLog::WriteLog(LogType::Critical, data, __VA_ARGS__)
#define ErrorLog(data, ...) CLog::WriteLog(LogType::Error, data, __VA_ARGS__)
#define WarningLog(data, ...) CLog::WriteLog(LogType::Warning, data, __VA_ARGS__)
#define InfoLog(data, ...) CLog::WriteLog(LogType::Info, data, __VA_ARGS__)
#define DebugLog(data, ...) CLog::WriteLog(LogType::Debug, data, __VA_ARGS__)

namespace bugat
{
class LogType
{
	public:
		enum en {
			Error = 0,
			Warning = 1,
			Info = 2,
			Debug = 3,
			Critical = 4,
		};

		static const char* GetString(LogType::en type)
		{
			switch (type)
			{
			case Error:
				return "Error";
			case Warning:
				return "Warning";
			case Info:
				return "Info";
			case Debug:
				return "Debug";
			case Critical:
				return "Critical";
			}
			return "";
		}
	};

	class CLog
	{
	public:
		template<typename... Args>
		static void WriteLog(LogType::en type, std::format_string<Args...> fmt, Args&&... args)
		{
			std::string s = std::format(fmt, std::forward<Args>(args)...);
			Write(type, s);
		}

	private:
		static bool Write(LogType::en type, std::string& log)
		{
			static std::mutex mu;
			std::lock_guard<std::mutex> lock(mu);

			auto nowTm = DateTime::NowTm();
			FILE* FilePtr = NULL;

			auto dateTimeStr = std::format("{}-{}-{} {}:{}:{}", nowTm.tm_year + 1900, nowTm.tm_mon + 1, nowTm.tm_mday, nowTm.tm_hour, nowTm.tm_min, nowTm.tm_sec);
			auto fileName = std::format("LOG_{}_{}_{}_{}.log", nowTm.tm_year + 1900, nowTm.tm_mon + 1, nowTm.tm_mday, nowTm.tm_hour);

			fopen_s(&FilePtr, fileName.c_str(), "a");
			if (!FilePtr)
				return false;

			std::print(FilePtr, "[{}] [{}] {}\n", dateTimeStr, LogType::GetString(type), log);
	#if defined(_DEBUG)
			std::print("[{}] [{}] {}\n", dateTimeStr, LogType::GetString(type), log);
	#else
			if (type != LogType::Debug)
				std::print("[{}] [{}] {}\n", dateTimeStr, LogType::GetString(type), log);
	#endif

			fflush(FilePtr);
			fclose(FilePtr);

			return true;
		}
	};
}