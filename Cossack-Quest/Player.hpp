#ifndef COSSACKQUEST_PLAYER_HPP
#define COSSACKQUEST_PLAYER_HPP

#include "Weapon.hpp"

struct Player {
	Vector3 playerPos;
	int health;
	int stamina;
	bool gameOver;
	unsigned int gold;
	Weapon weapon;
	bool attacking;
};

#endif //COSSACKQUEST_PLAYER_HPP
