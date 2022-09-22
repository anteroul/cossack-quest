#include "GameManager.hpp"

bool GameManager::wallCollision(Vector3 player, BoundingBox wallBox)
{
	BoundingBox playerBox = {player.x - 0.5f, player.y - 2.0f, player.z - 0.5f, player.x + 0.5f, player.y, player.z + 0.5f };

	return CheckCollisionBoxes(playerBox, wallBox);
}

int GameManager::prevWeapon(int index)
{
    if (weapon[index - 1].inInventory && index > 0)
        index--;

    return index;
}

int GameManager::nextWeapon(int index)
{
    if (weapon[index + 1].inInventory && index < 4)
        index++;

    return index;
}

bool GameManager::mouseWheelUp()
{
    return 0 < GetMouseWheelMove();
}

bool GameManager::mouseWheelDown()
{
    return 0 > GetMouseWheelMove();
}