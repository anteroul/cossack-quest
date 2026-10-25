#pragma once
#include <raylib.h>
#include "Weapon.hpp"

class PlayerControl
{
public:
	static bool wallCollision(Vector3 player, BoundingBox wallBox);
	static int prevWeapon(int index);
	static int nextWeapon(int index);
	static bool mouseWheelUp();
	static bool mouseWheelDown();
};

