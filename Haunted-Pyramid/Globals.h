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
	Vector3 playerSize;
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
	Vector3 position;
	Texture wallTexture;
	Model model;
	BoundingBox bounds;
} Wall;

static Player player;
static Camera camera{};
static Wall wall;

static Weapon weapon[5] = {
	"Torch", 30, 0,
	"Spear", 45, 50,
	"Bronze Sword", 50, 60,
	"Iron Sword", 100, 120,
	"Musket", 250, 300
};