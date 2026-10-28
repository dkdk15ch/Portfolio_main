#include "binFileLoader.h"
#include "SafeDelete.h"
#include <iostream>
#include <format>
#include "FileUtil.h"
#include <string_view>


namespace {
	uint32_t getUInt(std::ifstream& ifs) {
		char str[4];
		ifs.read(str, 4);
        return static_cast<uint8_t>(str[0]) |
            (static_cast<uint8_t>(str[1]) << 8) |
            (static_cast<uint8_t>(str[2]) << 16) |
            (static_cast<uint8_t>(str[3]) << 24);
    }
}

void Archive::open(const char* name)
{
    auto path = GetFilePath(name);

    m_Stream = std::make_unique<std::ifstream>(path, std::ifstream::binary);
    if (!m_Stream || !m_Stream->is_open()) {
        std::cerr << "[Archive] Failed to open: " << path << std::endl;
        return;
    }

    // テーブル先頭へ
    m_Stream->seekg(-4, std::ifstream::end);
    uint32_t tableBegin = getUInt(*m_Stream);
    std::cout << "[Debug] tableBegin = " << tableBegin << std::endl;

    m_Stream->seekg(tableBegin, std::ifstream::beg);
    m_FileNum = getUInt(*m_Stream);
    std::cout << "[Debug] m_FileNum = " << m_FileNum << std::endl;

    if (m_FileNum > 100000) {
        std::cerr << "[Error] m_FileNum is absurd: " << m_FileNum << std::endl;
        return;
    }

    m_Entries.clear();
    for (uint32_t i = 0; i < m_FileNum; ++i) {
        Entry e;
        e.position = getUInt(*m_Stream);
        e.size = getUInt(*m_Stream);
        uint32_t length = getUInt(*m_Stream);

        if (length < 0 || length > 4096) {
            std::cerr << "[Error] Corrupted length (" << length << ") at index " << i << std::endl;
            break;
        }

        std::vector<char> nameBuf(length);
        if (length > 0) {
            m_Stream->read(nameBuf.data(), length);
        }
        std::string key(nameBuf.data(), length);

        for (auto& c : key) {
            if (c == '\\') c = '/';
        }

        m_Entries.insert(std::make_pair(key, e));

        std::cout << std::format("[Debug] [{}] key='{}', pos={}, size={}\n",
            i, key, e.position, e.size);
    }
};

void Archive::read(char** outData, uint32_t* outSize, const char* name)
{
	auto it = m_Entries.find(name);
	if (it == m_Entries.end()) { return; }

	const Entry& e = it->second;
	*outData = new char[e.size];
	*outSize = e.size;
	m_Stream->seekg(e.position, std::ifstream::beg);
	m_Stream->read(*outData, e.size);
}

std::vector<char> Archive::read(const char* name)
{
    auto it = m_Entries.find(name);
    if (it == m_Entries.end()) { return std::vector<char>(); }

    const Entry& e = it->second;
    std::vector<char> data(e.size);
    if (e.size > 0) {
        m_Stream->seekg(e.position, std::ifstream::beg);
        m_Stream->read(data.data(), e.size);
    }
    return data;
}

std::unique_ptr<WavData> Archive::readWAV(const char* name)
{
    auto result = std::make_unique<WavData>();
    auto it = m_Entries.find(name);
    if (it == m_Entries.end()) { return std::move(result); }


    const Entry& e = it->second;
    if (e.size < 12) { return std::move(result); }

    result->fullData = std::make_unique<uint8_t[]>(e.size);
    auto& data = result->fullData;
    m_Stream->seekg(e.position, std::ifstream::beg);
    m_Stream->read(reinterpret_cast<char*>(data.get()), e.size);

    if ((std::string_view(reinterpret_cast<char*>(data.get()), 4) != "RIFF") ||
        (std::string_view(reinterpret_cast<char*>(data.get()+ 8), 4) != "WAVE")) {
        return result;
    }

    //  残り
    size_t offset = 12;
    std::string_view id;
    while (offset + 8 <= e.size) {
        const char* chunkStart = reinterpret_cast<const char*>(data.get() + offset);
        id = std::string_view(chunkStart, 4);
        uint32_t chunkSize = 0;
        std::memcpy(&chunkSize, chunkStart + 4, 4);
        
        if (id == "fmt ") {
            result->fxOffset = static_cast<int>(offset + 8);
        }
        else if (id == "data") {
            const char* audioStart = chunkStart + 8;
            result->dataOffset = offset + 8;
            result->dataSize = chunkSize;
            return std::move(result);
        }
        offset += 8 + chunkSize;
    }
    return result;
}
