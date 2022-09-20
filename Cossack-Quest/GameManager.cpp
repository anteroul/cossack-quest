#include "GameManager.hpp"

bool GameManager::wallCollision(Vector3 player, BoundingBox wallBox)
{
	BoundingBox playerBox = {player.x - 0.5f, player.y - 2.0f, player.z - 0.5f, player.x + 0.5f, player.y, player.z + 0.5f };
	return CheckCollisionBoxes(playerBox, wallBox);
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
    return 0 < GetMouseWheelMove();
}

bool GameManager::MouseWheelDown()
{
    return 0 > GetMouseWheelMove();
}
