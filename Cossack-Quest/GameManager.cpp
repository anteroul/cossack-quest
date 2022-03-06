#include "GameManager.h"

bool GameManager::wallCollision(Vector3 player, BoundingBox wallBox)
{
	BoundingBox playerBox = {player.x - 0.5f, player.y - 2.0f, player.z - 0.5f, player.x + 0.5f, player.y, player.z + 0.5f };

	if (CheckCollisionBoxes(playerBox, wallBox))
		return true;

	return false;
}