#include "Enemy.hpp"

Enemy::Enemy(Model model, Texture tex, Vector3 pos, Vector3 size, int hp, int dmg) : GameObject(pos, tex, model), active(true)
{
	health = hp;
	damage = dmg;
	enemyBoxSize = size;
	attacking = false;
	collisionWall = false;
	collisionPlayer = false;
	enemyBounds = BoundingBox{ enemyBoxSize.x, enemyBoxSize.y, enemyBoxSize.z };
	model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
}

Enemy::~Enemy()
{}

void Enemy::update()
{
	if (active)
	{
		if (health <= 0)
			active = false;

		enemyBounds = { enemyBoxSize.x, enemyBoxSize.y, enemyBoxSize.z };
	}
}
