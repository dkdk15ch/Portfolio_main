#pragma once
#include <fstream>
#include <unordered_map>
#include <memory>
#include <string>

struct Entry {
	int position;
	int size;
};

struct WavData {
	std::unique_ptr<uint8_t[]> fullData;
	int fxOffset = -1;
	size_t dataOffset;
	size_t dataSize;
};


class Archive {
public:
	void open(const char* name);
	void read(char** outData, uint32_t* outSize, const char* name);
	std::vector<char> read(const char* name);
	std::unique_ptr<WavData> readWAV(const char* name);

private:
	std::unique_ptr<std::ifstream> m_Stream;
	uint32_t m_FileNum;
	std::unordered_map<std::string, Entry> m_Entries;
};