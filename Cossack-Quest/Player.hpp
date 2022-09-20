#include "raylib.h"
#include "Globals.hpp"

class Player
{
public:
    Player();
    ~Player();
    void update();              
    void takeDamage(int dmg);
    void attack();
    bool gameOver;
    int weaponFrame;
    int cWeapon;
	int health;
	int stamina;
    unsigned int gold;
    Vector3 playerPos;
    Vector2 weaponPosition;
    Rectangle weaponRec;
    Weapon weapon;
private:
    unsigned int cFrame;
    bool attacking;
};