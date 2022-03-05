#pragma once

#include "iostream"
#include "raylib.h"

static int level = 0;

typedef struct Weapon {
	std::string name;
	int dmg;
	unsigned int price;
} Weapon;

typedef struct Player {
	Vector2 playerPos;
	int health;
	bool gameOver;
	unsigned int gold;
	Weapon weapon;
} Player;

typedef struct Enemy {
	Vector3 enemyPos;
	Vector3 enemyBoxSize;
	BoundingBox enemyBounds;
	bool active;
	int damage;
} Enemy;

typedef struct Wall {
	Texture wallTexture;
	Model model;
} Wall;

static Player player;
static Camera camera{};
static Wall wall;

static Weapon weapon[5] = {
	"Spear", 30, 0,
	"Bronze Sword", 45, 50,
	"Iron Sword", 50, 60,
	"Crossbow", 100, 120,
	"Musket", 250, 300
};

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