#pragma once
#include "GameObject/Enemy/Enemy.h"

class ModelRenderComp;
class Animator;
class Ball;

class Bat : public Enemy {
public:
	Bat();
	~Bat();

	Ball* attackBall()const { return m_AttackBall; }
	void resetAttackBall();
private:
	Animator* m_Animator;
	Ball* m_AttackBall;
};


