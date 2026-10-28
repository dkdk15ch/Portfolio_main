#pragma once
#include <vector>
#include <string>
#include <list>

namespace Archive{
	void create(const std::vector<std::string>& files, const std::string& output);
	void enumerateFiles(std::list<std::string>& fileNameList, const std::string directoryName);
}