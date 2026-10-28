#pragma once
#include <string>

class FileLog
{
public:
	static void initPointerLog();
	static void writePLog(const std::string& message);
	static void analyzePointerLog();
	
	enum class LOG_MODE
	{
		NONE,
		POINTER,
	};
	
	
	static std::wstring POINTER_LOG_PATH;
private:
	static LOG_MODE m_Mode;
};

