#include "GameManager.hpp"

bool GameManager::wallCollision(Vector3 player, BoundingBox wallBox)
{
	BoundingBox playerBox = {player.x - 0.5f, player.y - 2.0f, player.z - 0.5f, player.x + 0.5f, player.y, player.z + 0.5f };

	if (CheckCollisionBoxes(playerBox, wallBox))
		return true;

	return false;
}

int GameManager::prevWeapon()
{
    if (weapon[cWeapon - 1].inInventory && cWeapon > 0)
        cWeapon--;

    return cWeapon;
}

int GameManager::nextWeapon()
{
    if (weapon[cWeapon + 1].inInventory && cWeapon < 4)
        cWeapon++;

    return cWeapon;
}

bool GameManager::MouseWheelUp()
{
    if (0 < GetMouseWheelMove())
        return true;

    return false;
}

bool GameManager::MouseWheelDown()
{
    if (0 > GetMouseWheelMove())
        return true;

    return false;
}
