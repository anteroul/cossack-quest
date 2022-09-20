#include "Enemy.hpp"

Enemy::Enemy(Vector3 pos, float hp, float dmg) : enemyPos(pos), health(hp), damage(dmg), active(true)
{}

Enemy::~Enemy()
{}

void Enemy::update()
{
	if (active)
	{
		if (health <= 0)
			active = false;
	}
}
