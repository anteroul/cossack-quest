#pragma once

#include "iostream"
#include "raylib.h"

// debugging variables:
static bool noClip = false;
static bool debugCoordinates = true;

static unsigned int cFrame = 0;
static int level = 0;
static int cWeapon = 0;

typedef struct Weapon {
	std::string name;
	int dmg;
	unsigned int price;
	bool inInventory;
	bool melee;
	int ammo;
} Weapon;

typedef struct Player {
	Vector3 playerPos;
	int health;
	int stamina;
	bool gameOver;
	unsigned int gold;
	Weapon weapon;
	bool attacking;
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

typedef struct GameObject {
	Vector3 position;
	Texture texture;
	Model model;
} GameObject;

static Player player;
static Camera camera{};
static Wall wall;

static Texture2D hud;
static Texture2D weaponTexture[5];
static Vector2 weaponPosition;
static Rectangle weaponRec;
static int weaponFrame = 0;

static Weapon weapon[5] = {
	"Pitchfork", 30, 0, true, true, 1,
	"Bronze Sword", 45, 50, true, true, 1,
	"Iron Sword", 50, 60, true, true, 1,
	"Crossbow", 100, 120, true, false, 20,
	"Musket", 250, 300, true, false, 20
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