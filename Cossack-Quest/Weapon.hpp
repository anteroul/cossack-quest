#ifndef COSSACKQUEST_WEAPON_HPP
#define COSSACKQUEST_WEAPON_HPP

#include "iostream"

enum weaponID { PITCHFORK, SWORD_BRONZE, SWORD_IRON, CROSSBOW, MUSKET };

static int cWeapon = 0;
static Texture2D weaponTexture[5];
static Vector2 weaponPosition;
static Rectangle weaponRec;
static int weaponFrame = 0;

typedef struct Weapon {
	std::string name;
	int dmg;
	unsigned int price;
	bool inInventory;
	bool melee;
	int ammo;
} Weapon;

static Weapon weapon[5] = {
		"Pitchfork", 30, 0, true, true, 1,
		"Bronze Sword", 45, 50, true, true, 1,
		"Iron Sword", 50, 60, true, true, 1,
		"Crossbow", 100, 120, true, false, 20,
		"Musket", 250, 300, true, false, 20
};

#endif //COSSACKQUEST_WEAPON_HPP
