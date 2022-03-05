#pragma once

#include "raylib.h"
#include "stdlib.h"
#include "Globals.h"
#include "GameManager.h"

class Game
{
public:
	Game();
	~Game();
	void runApplication();
	void deInit();
private:
	void update();
	void initGame();
	void draw();
	void resetGame();
	BoundingBox* walls[64];
};