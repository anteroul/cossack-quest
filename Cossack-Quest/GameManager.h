#pragma once
#include "raylib.h"

class GameManager
{
public:
	static bool wallCollision(Vector3 player, BoundingBox wallBox);
};

