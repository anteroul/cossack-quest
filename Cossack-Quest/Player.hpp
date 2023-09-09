#ifndef COSSACKQUEST_PLAYER_HPP
#define COSSACKQUEST_PLAYER_HPP

#include <raylib.h>
#include "Weapon.hpp"

struct Player {
	Vector3 playerPos;
	int health;
	int stamina;
	bool gameOver;
	bool hit;
	unsigned int gold;
	Weapon weapon;
	bool attacking;
	Sound* death;

	void takeDamage(int dmg)
	{
		health -= dmg;
		hit = true;
		if (health <= 0)
		{
			PlaySound(*death);
			health = 0;
			gameOver = true;
		}
	}
};

#endif //COSSACKQUEST_PLAYER_HPP
