#pragma once
#include <cstdint>

class GameManager {
public:
	static inline GameManager* m_Instance = nullptr;

	GameManager() {
		if (m_Instance) { return; }
		m_Instance = this;
	}
	virtual ~GameManager() = default;
	virtual void bind() = 0;	
	virtual void update(double deltaTime) {};
	
	void setDeltaTime(double delta) { m_DeltaTime = delta; }
	double deltaTime()const { return m_DeltaTime; }

private:
	double m_DeltaTime = 0.0;
};