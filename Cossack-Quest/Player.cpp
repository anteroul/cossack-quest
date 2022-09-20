#include "Player.hpp"

Player::Player()
{
    stamina = 100;
    health = 100;
    gold = 0;
    gameOver = false;
    playerPos = {0.0f, 4.0f, 0.0f};
    weapon = weapon[cWeapon];
    attacking = false;
    weaponPosition = {GetScreenWidth() * 0.325f, GetScreenHeight() * 0.3f};
    weaponRec = {0, 0, (float)weaponTexture[0].width / 4, (float)weaponTexture[0].height / 2};
    cWeapon = 0;
    weaponFrame = 0;
}

Player::~Player()
{
}

void Player::update()
{
    weapon = weapon[cWeapon];

    if (!gameOver)
    {
        if (health <= 0)
            gameOver = true;

        if (!attacking && stamina < 100)
            stamina++;

        if (attacking)
        {
            if (cWeapon == SWORD_BRONZE || cWeapon == SWORD_IRON)
                weaponPosition = {GetScreenWidth() * 0.325f, GetScreenHeight() * 0.6f};

            weaponRec.x = 400.0f * weaponFrame;
            weaponRec.y = 400.0f;

            if (weaponFrame > 3)
            {
                weaponFrame = 0;
                weaponRec.y = 0.0f;
                weaponPosition = {GetScreenWidth() * 0.325f, GetScreenHeight() * 0.3f};
                attacking = false;
            }
            if (cFrame == 5 && weaponFrame < 4)
            {
                weaponFrame++;
                cFrame = 0;
            }
            cFrame++;
        }
        
        else if (cWeapon == SWORD_BRONZE || cWeapon == SWORD_IRON && !attacking)
            weaponPosition = {GetScreenWidth() * 0.325f, GetScreenHeight() * 0.325f};
        else
            weaponPosition = {GetScreenWidth() * 0.325f, GetScreenHeight() * 0.3f};
    }
}

void Player::takeDamage(int dmg)
{
    health -= dmg;
}

void Player::attack()
{
    if (stamina >= 30 && weapon.melee)
    {
        attacking = true;
        stamina -= 30;
        weaponFrame++;
    }
    if (weapon.ammo > 0 && !weapon.melee)
    {
        attacking = true;
        weaponFrame++;
        weapon[cWeapon].ammo--;
    }
}