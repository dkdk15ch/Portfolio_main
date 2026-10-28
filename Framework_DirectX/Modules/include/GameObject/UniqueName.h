#pragma once
#include <string>
#include <format>
#include <cstdint>

class UniqueName {
	friend class LevelManager;
public:
	virtual ~UniqueName() = default;

protected:
	static std::string makeUniqueName(const std::string& name) 
	{
		uint32_t count = m_NameMap[name]++;
		return std::format("{}_{:02}", name, count);
	}

private:
	static void clearMap() { m_NameMap.clear(); }
	static inline std::unordered_map<std::string, uint32_t> m_NameMap;
};
