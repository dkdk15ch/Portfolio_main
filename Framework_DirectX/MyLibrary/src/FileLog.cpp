#include "FileLog.h"
#include <unordered_map>
#include <fstream>
#include <format>
#include <filesystem>
#include "FileUtil.h"
#include <array>

std::wstring FileLog::POINTER_LOG_PATH;
FileLog::LOG_MODE FileLog::m_Mode = LOG_MODE::NONE;

void FileLog::initPointerLog()
{
	m_Mode = LOG_MODE::POINTER;

	std::wstring path = GetFilePathW(L"project/Log/PointerLog.txt");
	POINTER_LOG_PATH = path;
	std::ofstream ofs_PLog(POINTER_LOG_PATH, std::ios::out | std::ios::trunc);
}

void FileLog::analyzePointerLog() {
	if(m_Mode != LOG_MODE::POINTER){ return;}

	std::ifstream ifs(POINTER_LOG_PATH);
	if (!ifs.is_open()) { return; }

	std::string line;
	// 1行ずつ読み込む

	std::unordered_map<std::string, int> makeCount;
	std::unordered_map<std::string, int> deleteCount;

	while (std::getline(ifs, line)) {
		std::stringstream ss(line);
		std::string segment;
		std::vector<std::string> parts;
		while (ss >> segment) {
			parts.push_back(segment);
		}

		if (line[0] == 'M') {
			if (parts.size() <= 2) {
				writePLog("=== Error ===\n");
				continue;
			}
			makeCount[parts[2]] += 1;
		}
		if (line[0] == 'D') {
			if (parts.size() <= 2) {
				writePLog("=== Error ===\n");
				continue;
			}
			deleteCount[parts[2]] += 1;
		}
	}
	ifs.close();

	writePLog("=== Result ===\n");
	std::unordered_map<std::string, std::array<int, 2>> result;

	for (const auto& [key, value] : makeCount) {
		result[key][0] = value;
	}

	for (const auto& [key, value] : deleteCount) {
		result[key][1] = value;
	}

	for (const auto& [key, array] : result) {
		writePLog(std::format("{:<20} : M={} D={}\n", key, array[0],
			array[1]));
	}

}

void FileLog::writePLog(const std::string& message)
{
	if(m_Mode != LOG_MODE::POINTER){ return;}

	static std::ofstream ofs_Plog(POINTER_LOG_PATH, std::ios::app);
	if (ofs_Plog.is_open()) {
		ofs_Plog << message; 
		ofs_Plog.flush();
	}
}
