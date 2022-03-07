#include "GameManager.h"

bool GameManager::wallCollision(Vector3 player, BoundingBox wallBox)
{
	BoundingBox playerBox = {player.x - 0.5f, player.y - 2.0f, player.z - 0.5f, player.x + 0.5f, player.y, player.z + 0.5f };

	if (CheckCollisionBoxes(playerBox, wallBox))
		return true;

	return false;
}

int GameManager::prevWeapon()
{
    if (cWeapon > 0)
    {
        if (weapon[cWeapon - 1].inInventory)
            cWeapon--;
    }
    else
    {
        for (int i = 4; i >= 0; i--)
        {
            if (weapon[i].inInventory && weapon[cWeapon].ammo > 0)
            {
                cWeapon = i;
                break;
            }
        }
    }
    return cWeapon;
}


int GameManager::nextWeapon()
{
    if (weapon[cWeapon + 1].inInventory)
        cWeapon++;
    else
        cWeapon = 0;

    return cWeapon;
}

