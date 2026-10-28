#pragma once
#include <cstdint>
#include <mutex>
#include <cassert>
#include <functional>
#include "Texture.h"

//	プール用テンプレートクラス
template<typename T>
class Pool
{
public:
	Pool()
		: m_PBuffer(nullptr)
		, m_PActive(nullptr)
		, m_PFree(nullptr)
		, m_Capacity(0)
		, m_Count(0)
	{/* DO_NOTHIN */
	}

	~Pool()
	{
		term();
	}

	bool init(uint32_t count)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		m_PBuffer = static_cast<uint8_t*>(malloc(sizeof(Item) * (count * 2)));
		if (m_PBuffer == nullptr){
			return false;
		}

		m_Capacity = count;

		//	インデックスを振る
		for (auto i = 2u, j = 0u; i < m_Capacity + 2; ++i, ++j)	{
			auto item = getItem(i);
			item->m_Index = j;
		}

		m_PActive = getItem(0);
		m_PActive->m_PPrev = m_PActive->m_PNext = m_PActive;
		m_PActive->m_Index = uint32_t(-1);

		m_PFree = getItem(1);
		m_PFree->m_Index = uint32_t(-2);

		for (auto i = 1u; i < m_Capacity + 2; ++i){
			getItem(i)->m_PPrev = nullptr;
			getItem(i)->m_PNext = getItem(i + 1);
		}

		getItem(m_Capacity + 1)->m_PPrev = m_PFree;
		m_Count = 0;

		return true;
	}


	/*
	 * @brief 終了処理
	 */
	void term()
	{
		std::lock_guard <std::mutex > guard(m_Mutex);

		if (m_PBuffer){
			free(m_PBuffer);
			m_PBuffer = nullptr;
		}

		m_PActive = nullptr;
		m_PFree = nullptr;
		m_Capacity = 0;
		m_Count = 0;
	}

	/**
	 * @brief			アイテムを確保する
	 * @param func		ユーザーによる初期化処理
	 * @return			確保したアイテムへのポインタ、失敗したらnullptr
	 */
	T* Alloc(std::function<void(uint32_t, T*)> func = nullptr)
	{
		std::lock_guard<std::mutex> gaurd(m_Mutex);

		if (m_PFree->m_PNext == m_PFree || m_Count + 1 > m_Capacity){
			return nullptr;
		}

		auto item = m_PFree->m_PNext;
		m_PFree->m_PNext = item->m_PNext;

		item->m_PPrev = m_PActive->m_PPrev;
		item->m_PNext = m_PActive;
		item->m_PPrev->m_PNext = item->m_PNext->m_PPrev = item;

		m_Count++;

		//	メモリ割り当て
		auto val = new ((void*)item) T();

		//	初期化の必要があれば呼ぶ
		if (func != nullptr){
			func(item->m_Index, val);
		}

		return val;
	}

	/**
	 * @brief			アイテムを解放する
	 * @param pValue 	pValue = 解放するアイテムへのポインタ
	 */
	void Free(T* pValue)
	{
		if (pValue == nullptr){
			return;
		}

		std::lock_guard<std::mutex> guard(m_Mutex);

		auto item = reinterpret_cast<Item*>(pValue);

		item->m_PPrev->m_PNext = item->m_PNext;
		item->m_PNext->m_PPrev = item->m_PPrev;

		item->m_PPrev = nullptr;
		item->m_PNext = m_PFree->m_PNext;

		m_PFree->m_PNext = item;
		m_Count--;

	}

	/**
	 * @brief	総アイテム数を取得
	 * @return	総アイテム数を返す
	 */
	uint32_t getSize() const { return m_Capacity; }

	uint32_t getUsedCount() const { return m_Count; }

	/**
	 * @brief	利用可能なアイテム数を取得
	 * @return	利用可能なアイテム数を返却
	 */
	uint32_t getAvailableCount() const { return m_Capacity - m_Count; }

private:
	// ========================================================
	// Imte structure
	// ========================================================
	struct Item
	{
		T			m_Value;	//	値
		uint32_t	m_Index;	//	インデックス
		Item*		m_PNext;	//	次のアイテムへのポインタ
		Item*		m_PPrev;	//	前のアイテムへのポインタ

		Item()
		: m_Value()
		, m_Index(0)
		, m_PNext(nullptr)
		, m_PPrev(nullptr)
		{ /* DO_NOTHING */ }

		~Item()
		{/* DO_NOTHING */ }
	};

	uint8_t*	m_PBuffer;		//	バッファ
	Item*		m_PActive;		//	アクティブアイテムの先頭
	Item*		m_PFree;		//	フリーアイテムの先頭
	uint32_t	m_Capacity;		//	総アイテム数
	uint32_t	m_Count;		//	確保したアイテム数
	std::mutex	m_Mutex;		//	ミューテックス
	
	/**
	 * @brief			アイテムを取得
	 * @param index		取得するアイテムのインデックス
	 * @return			アイテムへのポインタを返す
	 */
	Item* getItem(uint32_t index)
	{
		assert(0 <= index && index <= m_Capacity + 2 && "Pool::GetItem");

		return reinterpret_cast<Item*>(m_PBuffer + sizeof(Item) * index);
	}

	Item* assignItem(uint32_t index)
	{
		assert(0 <= index && index <= m_Capacity + 2 && "Pool::AssignItem");
		auto buf = (m_PBuffer + sizeof(Item) * index);
		return new (buf)Item;
	}

	Pool(const Pool&) = delete;
	void operator = (const Pool&) = delete;

};
