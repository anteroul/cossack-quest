#pragma once
#include <raylib.h>
#include "Weapon.hpp"

class PlayerControl
{
public:
    static void placeCursorMiddle(Vector2 screenSize, Vector2 mousePos);
	static bool wallCollision(Vector3 player, BoundingBox wallBox);
	static int prevWeapon(int index);
	static int nextWeapon(int index);
	static bool mouseWheelUp();
	static bool mouseWheelDown();
};

