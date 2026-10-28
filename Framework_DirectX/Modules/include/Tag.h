#pragma once
#include <cstdint>

enum class CollisionTag : uint32_t {
	disable		= 1u << 0,	//	無効時
	Character	= 1u << 1,	//	キャラ
	Static		= 1u << 2,	//	静止物
	Dynamic		= 1u << 3,	//	移動物
	//	固有タグ
	Player		= 1u << 4,	
	Ball		= 1u << 5,	
	Block		= 1u << 6,
	Racket		= 1u << 7,
	Enemy		= 1u << 8,
	EnemyBall	= 1u << 9,
	Item		= 1u << 10,
	TargetUI_Ray = 1u << 11,
};

template <typename TagEnum>
class Tag {
public:
	void setTag(uint32_t newValue) {
		m_Tag = newValue;
	}

	void addTag(TagEnum tag) {
		m_Tag |= static_cast<uint32_t>(tag);
	}

	void removeTag(TagEnum tag) {
		m_Tag &= ~static_cast<uint32_t>(tag);
	}

	bool hasTag(TagEnum tag) const {
		return (m_Tag & static_cast<uint32_t>(tag));
	}

	uint32_t tag() const { return m_Tag; }

private:
	uint32_t m_Tag = 0;
};
