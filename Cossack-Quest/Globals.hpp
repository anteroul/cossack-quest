#pragma once

#include "GameObject.hpp"
#include "Player.hpp"

// debugging variables:
static bool noClip = false;
static bool debugCoordinates = true;

static unsigned int cFrame = 0;
static int level = 0;

static Player player;
static Camera camera{};
static GameObject wall;
static Texture2D hud;


static int map[8][8] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 1, 0, 0, 0, 1,
	1, 0, 0, 1, 0, 1, 0, 1,
	1, 0, 0, 0, 0, 1, 0, 1,
	1, 1, 1, 0, 1, 1, 0, 1,
	1, 0, 1, 0, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1
};