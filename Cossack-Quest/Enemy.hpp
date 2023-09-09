#ifndef COSSACKQUEST_ENEMY_HPP
#define COSSACKQUEST_ENEMY_HPP

#include <array>
#include <iostream>
#include <vector>
#include <limits>
#include <queue>
#include <unordered_map>
#include "GameObject.hpp"
#include "Player.hpp"
#include "PlayerControl.hpp"

#define ROWS 8
#define COLS 8


class Enemy : public GameObject
{
public:
	Enemy(Vector3 pos, Texture tex, Model mod, std::array<BoundingBox*, 64> wa);
	~Enemy();
	void update(Player* target);

	Vector3 position{};
	Texture texture;
	Model model;
private:
	std::vector<std::vector<int>> dijkstra(int map[ROWS][COLS], int startRow, int startCol, int endRow, int endCol, std::vector<std::vector<int>>& distance);
	std::vector<std::pair<int, int>> backtrack(int startRow, int startCol, int endRow, int endCol, const std::vector<std::vector<int>>& distance);

	const int INF = std::numeric_limits<int>::max(); // Define infinity
	std::array<BoundingBox*, 64> wallArray;

	struct Node {
		int row, col, cost;
	};

	bool alive;
	bool attacking;
	float yaw;
	int playerX, playerY;
	int enemyX, enemyY;
};


#endif //COSSACKQUEST_ENEMY_HPP
