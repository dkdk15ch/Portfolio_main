#pragma once
#include <random>

class MyRand {
public:
	static inline MyRand* m_Instance = nullptr;
	MyRand() {
		if (m_Instance) { return; }
		m_Instance = this;
		setGen();
	}

	static int range(int min, int max) {
		std::uniform_int_distribution<int> dist(min, max);
		return dist(m_Gen);
	}
	static float range(float min, float max) {
		std::uniform_real_distribution<float> dist(min, max);
		return dist(m_Gen);
	}

private:
	static inline std::mt19937 m_Gen;
	void setGen() {
		std::random_device d;
		m_Gen = std::mt19937(d());
	}
};