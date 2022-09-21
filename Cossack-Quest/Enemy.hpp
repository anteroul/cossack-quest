#ifndef COSSACKQUEST_ENEMY_HPP
#define COSSACKQUEST_ENEMY_HPP

#include "GameObject.hpp"

class Enemy : public GameObject
{
public:
	Enemy(Model model, Texture tex, Vector3 pos, Vector3 size, int hp, int dmg);
	~Enemy();
	void update();
	BoundingBox enemyBounds;
private:
	Vector3 enemyBoxSize;
	bool active;
	bool attacking;
	int health;
	int damage;
};


#endif //COSSACKQUEST_ENEMY_HPP
