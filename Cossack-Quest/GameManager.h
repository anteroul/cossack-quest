#pragma once
#include "raylib.h"
#include "Globals.h"

class GameManager
{
public:
	static bool wallCollision(Vector3 player, BoundingBox wallBox);
	static int prevWeapon();
	static int nextWeapon();
};

