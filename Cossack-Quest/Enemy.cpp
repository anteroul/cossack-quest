#include "Enemy.hpp"

#define NORTH 0
#define NORTHEAST 1
#define EAST 2
#define SOUTHEAST 3
#define SOUTH 4
#define SOUTHWEST 5
#define WEST 6
#define NORTHWEST 7


Enemy::Enemy(Vector3 pos, Texture tex, Model mod, BoundingBox* wa) : GameObject(pos, tex, mod)
{
	position = pos;
	texture = tex;
	model = mod;
	wallArray = wa;
	wallIndex = nullptr;
	alive = true;
}

Enemy::~Enemy() = default;


void Enemy::update()
{

}

unsigned Enemy::findPath()
{
	int direction = NORTH;
	int distance[8] = {};
	int shortestRoute = -1;
	BoundingBox box = {{position.x * 8.f - 10.f, 0.f, position.z * 8.f - 10.f}, {position.x * 8.f - 6.f, 4.f, position.z * 8.f - 6.f}};

	for (;;)
	{
		if (PlayerControl::wallCollision(position, box))
		{

		}
	}

	return direction;
}

bool Enemy::checkCollisionPlayer(BoundingBox &box)
{
	return false;
}
