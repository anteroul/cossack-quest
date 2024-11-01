#ifndef COSSACKQUEST_WEAPON_HPP
#define COSSACKQUEST_WEAPON_HPP

#include <iostream>

enum weaponID { PITCHFORK, SWORD_BRONZE, SWORD_IRON, CROSSBOW, MUSKET };

struct Weapon {
	std::string name;
	int dmg;
	unsigned int price;
	bool inInventory;
	bool melee;
	int ammo;
};

static Weapon weapon[5] = {
		"Pitchfork", 30, 0, true, true, 1,
		"Bronze Sword", 45, 50, true, true, 1,
		"Steel Sword", 50, 60, true, true, 1,
		"Crossbow", 100, 120, true, false, 20,
		"Musket", 250, 300, true, false, 20
};

#endif //COSSACKQUEST_WEAPON_HPP
