#ifndef COSSACKQUEST_PLAYER_HPP
#define COSSACKQUEST_PLAYER_HPP

#include <raylib.h>
#include "Weapon.hpp"

struct Player {
	Vector3 playerPos;
	int health;
	int stamina;
	bool gameOver;
	unsigned int gold;
	Weapon weapon;
	bool attacking;

	Player() = default;
};

#endif //COSSACKQUEST_PLAYER_HPP
