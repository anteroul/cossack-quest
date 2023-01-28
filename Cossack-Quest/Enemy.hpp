#ifndef COSSACKQUEST_ENEMY_HPP
#define COSSACKQUEST_ENEMY_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include "GameObject.hpp"
#include "PlayerControl.hpp"

class Enemy : public GameObject
{
public:
	Enemy(Vector3 pos, Texture tex, Model mod, BoundingBox* wa);
	~Enemy();
	void update();

	Vector3 position{};
	Texture texture;
	Model model;
private:

	struct Node {
		int x, y;
		int f, g, h;

		bool operator<(const Node& other) const {
			return f > other.f;
		}
	};

	bool alive;
	BoundingBox* wallArray;
	BoundingBox* wallIndex;

	static int heuristic(int x1, int y1, int x2, int y2);
	static bool isValid(int x, int y, int rows, int cols);
	void a_star(std::vector<std::vector<int>>& grid, Node start, Node goal);
	unsigned findPath();
};


#endif //COSSACKQUEST_ENEMY_HPP
