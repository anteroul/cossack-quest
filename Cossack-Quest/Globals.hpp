#pragma once

#include "iostream"
#include "raylib.h"

enum weaponID { PITCHFORK, SWORD_BRONZE, SWORD_IRON, CROSSBOW, MUSKET };

// debugging variables:
static bool noClip = false;
static bool debugCoordinates = true;

static int level = 0;

typedef struct Weapon {
	std::string name;
	int dmg;
	unsigned int price;
	bool inInventory;
	bool melee;
	int ammo;
} Weapon;

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

typedef struct GameObject {
	Vector3 position;
	Texture texture;
	Model model;
} GameObject;

typedef struct Level {
	int map[8][8];
} Level;

static Camera camera{};
static Wall wall;
static Texture2D hud;
static Texture2D weaponTexture[5];

static Weapon weapon[5] = {
	"Pitchfork", 30, 0, true, true, 1,
	"Bronze Sword", 45, 50, true, true, 1,
	"Iron Sword", 50, 60, true, true, 1,
	"Crossbow", 100, 120, true, false, 20,
	"Musket", 250, 300, true, false, 20
};