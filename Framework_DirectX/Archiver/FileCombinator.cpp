#include "FileCombinator.h"
#include <iostream>
#include <ostream>
#include <fstream>
#include "SafeDelete.h"
#include <Windows.h>
#include <format>
#include "FileUtil.h"

namespace {
	void write(std::ofstream* o, uint32_t a) {
		char str[4];
		str[0] = static_cast<char>(a & 0x000000ff);
		str[1] = static_cast<char>((a & 0x0000ff00) >> 8);
		str[2] = static_cast<char>((a & 0x00ff0000) >> 16);
		str[3] = static_cast<char>((a & 0xff000000) >> 24);
		o->write(str, 4);
	}

	//	無視する拡張子 （ドット込み）
	std::vector<std::string> ignoreExtensionList = {
		".pdn",
		".bas",
		".xlsm"
	};

	bool isIgnoredExtension(const std::string& filename) {
		for (const auto& ignore : ignoreExtensionList) {
			if (!filename.ends_with(ignore)) {
				continue;
			}
			return true;
		}

		return false;
	}
}

void Archive::create(const std::vector<std::string>& files, const std::string& output)
{
	// filesを開いて、コピー
 	std::ofstream out(output, std::ofstream::binary);

	uint32_t numFile = static_cast<uint32_t>(files.size());
	std::vector<uint32_t> fileSizes(files.size());
	for (uint32_t i = 0; i < files.size(); ++i) {
		std::ifstream in(files[i], std::ifstream::binary);
		if (!in.is_open()) {
			std::cout << "OPEN FAILED for file: [" << files[i] << "]\n";
			continue;
		}
		

		in.seekg(0, std::ifstream::end);
		uint32_t size = static_cast<uint32_t>(in.tellg());
		fileSizes[i] = size;
		in.seekg(0, std::ifstream::beg);
		char* data = new char[size];
		in.read(data, size);
		out.write(data, size);
		SAFE_DELETE_ARRAY(data);
	}

	uint32_t dataEnd = static_cast<uint32_t>(out.tellp());
	write(&out, numFile);
	uint32_t pos = 0;
	for (uint32_t i = 0; i < files.size(); ++i) {
		write(&out, pos);
		write(&out, fileSizes[i]);
		std::string trimPath = TrimBefore(files[i], "res");
		int length = static_cast<int>(trimPath.size());
		write(&out, static_cast<uint32_t>(length));
		out.write(trimPath.c_str(), length);
		uint32_t size = fileSizes[i];

		std::cout << std::format("{}\t{}\t{}\t{}\n",
			pos, size, length, trimPath);
		pos += fileSizes[i];
	}
	write(&out, dataEnd);   // 末尾に表の開始位置
}

void Archive::enumerateFiles(std::list<std::string>& fileNameList, const std::string directoryName)
{
	std::string searchPath = directoryName + "\\*.*";
	WIN32_FIND_DATAA fileData;

	HANDLE hFind = FindFirstFileA(searchPath.c_str(), &fileData);
	if (hFind == INVALID_HANDLE_VALUE) { return; }

	bool hasNext = true;
	while (hasNext) {
		std::string name = fileData.cFileName;
		if (name != "." && name != "..") {
			std::string fullpath = directoryName + "\\" + name;

			if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				std::cout << "Directory : " << fullpath << std::endl;
				enumerateFiles(fileNameList, fullpath);
			}
			else {
				//	除外チェック
				if (!isIgnoredExtension(name)) {
					std::cout << "\tFile : " << fullpath<< std::endl;
					fileNameList.push_back(fullpath);
				}
			}
		}
		hasNext = (FindNextFileA(hFind, &fileData) != FALSE);
	}

	FindClose(hFind);
}


