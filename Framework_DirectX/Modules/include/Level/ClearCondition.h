#pragma once

enum class GameCondition {
	Playing,
	Clear,
	Next,
	GameOver,
};

class ClearCondition {
public:
	ClearCondition() {};
	virtual ~ClearCondition() = default;

	virtual void init() = 0;
	
	GameCondition check() {
		//if (checkClear()) { return GameCondition::Clear; }
		//if (checkGameOver()) { return GameCondition::GameOver; }
		//return GameCondition::Playing;

		return m_CurrentCondition;
	};
protected:
	virtual bool checkClear() const  = 0;
	virtual bool checkGameOver() const { return false; };
	
	void setCondition(GameCondition newValue) { m_CurrentCondition = newValue; }
	
	GameCondition m_CurrentCondition =GameCondition::Playing;
};

