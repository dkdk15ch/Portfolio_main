#pragma once
#include <vector>
#include <string>
#include <unordered_map>
/**
 * @brief		マップ付きVecotr
 *				所有権なし
 */
template <typename T>
class MappedVector {
public:
	explicit MappedVector(uint32_t size) {
		m_List.reserve(size);
		m_KeyList.reserve(size);
	}

	bool add(const std::string& key, T* adder);
	bool remove(const std::string& key);
	void clear();

	const std::vector<T*>& list() const { return m_List; }
	uint32_t indexFromKey(const std::string& key)const;

private:
	std::vector<T*> m_List;
	std::vector<std::string> m_KeyList;
	std::unordered_map<std::string, uint32_t> m_Map;
};

template<typename T>
bool MappedVector<T>::add(const std::string& key, T* adder)
{
	if (!adder || m_Map.contains(key)) { 
		return false; }

	m_Map[key] = static_cast<uint32_t>(m_List.size());
	m_List.push_back(adder);
	m_KeyList.push_back(key);
	return true;
}

template<typename T>
bool MappedVector<T>::remove(const std::string& key) {
	auto it = m_Map.find(key);
	if (it == m_Map.end()) { return false; }
	//	末尾と入れ替えて、削除する。
	uint32_t removeIndex = it->second;
	uint32_t lastIndex = static_cast<uint32_t>(m_List.size() - 1);
	std::string lastKey = m_KeyList[lastIndex];
	std::swap(m_List[removeIndex], m_List[lastIndex]);
	std::swap(m_KeyList[removeIndex], m_KeyList[lastIndex]);
	m_Map[lastKey] = removeIndex;
	m_List.pop_back();
	m_KeyList.pop_back();
	m_Map.erase(it);
	return true;
}

template<typename T>
void MappedVector<T>::clear() {
	m_List.clear();
	m_KeyList.clear();
	m_Map.clear();
}

template<typename T>
inline uint32_t MappedVector<T>::indexFromKey(const std::string& key) const
{
	auto it = m_Map.find(key);
	if (it == m_Map.end()) { return UINT32_MAX; }

	return it->second;
}
