#pragma once
#include "DateTime.h"
#include "Singleton.h"

#include <mutex>
#include <stdio.h>
#include <stdarg.h>
#include <sstream>
#define MAX_LOG_LENGTH 1024

#define ErrorLog(data, ...) CLog::WriteLog(LogType::Error, data, __VA_ARGS__)
#define InfoLog(data, ...) CLog::WriteLog(LogType::Info, data, __VA_ARGS__)

namespace bugat
{
class LogType
{
	public:
		enum en {
			Error = 0,
			Warnning = 1,
			Info = 2
		};

		static const char* GetString(LogType::en type)
		{
			switch (type)
			{
			case Error:
				return "Error";
			case Warnning:
				return "Warnning";
			case Info:
				return "Info";
			}
			return "";
		}
	};

	class CLog
	{
	public:
		static bool WriteLog(LogType::en type, const char* data, ...)
		{
			static std::mutex mu;
			std::lock_guard<std::mutex> lock(mu);

			auto nowTm = DateTime::NowTm();
			FILE* FilePtr = NULL;

			char Log[MAX_LOG_LENGTH] = { 0, };

			va_list ap;
			va_start(ap, data);
			vsprintf_s(Log, data, ap);
			va_end(ap);

			auto dateTimeStr = std::format("%04d-%02d-%02d %02d:%02d:%02d", nowTm.tm_year, nowTm.tm_mon + 1, nowTm.tm_mday, nowTm.tm_hour, nowTm.tm_min, nowTm.tm_sec);
			auto fileName = std::format("LOG_%04d_%02d_%02d_%02d.log", nowTm.tm_year, nowTm.tm_mon + 1, nowTm.tm_mday, nowTm.tm_hour);

			fopen_s(&FilePtr, fileName.c_str(), "a");
			if (!FilePtr)
				return false;

			fprintf(FilePtr, "[%s] [%s] %s\n", dateTimeStr, LogType::GetString(type), Log);
	#if defined(_DEBUG)
			printf("[%s] [%s] %s\n", dateTimeStr, LogType::GetString(type), Log);
	#endif

			fflush(FilePtr);
			fclose(FilePtr);

			return true;
		}
	};
}