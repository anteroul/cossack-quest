#pragma once
#include "raylib.h"
#include "Weapon.hpp"

class GameManager
{
public:
	static bool wallCollision(Vector3 player, BoundingBox wallBox);
	static int prevWeapon();
	static int nextWeapon();
	static bool mouseWheelUp();
	static bool mouseWheelDown();
};

