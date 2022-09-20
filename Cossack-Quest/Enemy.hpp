#ifndef COSSACKQUEST_ENEMY_HPP
#define COSSACKQUEST_ENEMY_HPP

#include "raylib.h"

class Enemy
{
public:
	Enemy(Vector3 pos, float hp, float dmg);
	~Enemy();
	void update();
	Texture2D texture;
	Vector3 enemyPos;
private:
	Vector3 enemyBoxSize{};
	BoundingBox enemyBounds{};
	bool active{};
	float health{};
	float damage{};
};


#endif //COSSACKQUEST_ENEMY_HPP
