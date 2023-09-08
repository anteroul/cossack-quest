#include "Enemy.hpp"
#include <cmath>
#include <raymath.h>

#define NORTH 0
#define NORTHEAST 1
#define EAST 2
#define SOUTHEAST 3
#define SOUTH 4
#define SOUTHWEST 5
#define WEST 6
#define NORTHWEST 7


Enemy::Enemy(Vector3 pos, Texture tex, Model mod, std::array<BoundingBox*, 64> wa) : GameObject(pos, tex, mod)
{
	position = pos;
	texture = tex;
	model = mod;
	wallArray = wa;
	attacking = false;
	alive = true;
    yaw = 0.0f;
}

Enemy::~Enemy() = default;


void Enemy::update(Player* target)
{
    BoundingBox box = {{position.x * 8.f - 10.f, 0.f, position.z * 8.f - 10.f}, {position.x * 8.f - 6.f, 4.f, position.z * 8.f - 6.f}};
    Vector2 direction = { position.x - target->playerPos.x, position.z - target->playerPos.z };
    float angle = atan2(direction.y, direction.x) * RAD2DEG;

    // Define mapGrid
    int map[8][8]{};
    int iterator = 0;

    std::cout << "\n\n";

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            if (wallArray[iterator] != nullptr)
                map[x][y] = 1;
            else
            {
                if (target->playerPos.x > x * 8.0f - 12.0f && target->playerPos.x < x * 8.0f - 4.0f && target->playerPos.z > y * 8.0f - 12.0f && target->playerPos.z < y * 8.0f - 4.0f)
                {
                    map[x][y] = 2;
                } else {
                    map[x][y] = 0;
                }
            }
            iterator++;
            std::cout << map[x][y] << ",";
        }
        std::cout << "\n";
    }

    std::cout << "\n";

	if (alive)
	{
        if (target->health > 0)
        {
            /* TODO
		    switch (findPath())
		    {
			    default:
			    	break;
		    }
            */
        }
        if (angle > yaw + 90)
            yaw += 0.5f;
        if (angle < yaw + 90)
            yaw -= 0.5f;

        model.transform = MatrixRotateXYZ({0.0f, DEG2RAD*yaw*(-1), 0.0f});
	}
}

unsigned Enemy::findPath()
{
	int direction = NORTH;
	int distance = 0;
	int shortestRoute = -1;
	BoundingBox box = {{position.x * 8.f - 10.f, 0.f, position.z * 8.f - 10.f}, {position.x * 8.f - 6.f, 4.f, position.z * 8.f - 6.f}};

	for (;;)
	{
		// TODO
		break;
	}

	return direction;
}

int Enemy::heuristic(int x1, int y1, int x2, int y2)
{
	return abs(x1 - x2) + abs(y1 - y2);
}

bool Enemy::isValid(int x, int y, int rows, int cols)
{
	return x >= 0 && y >= 0 && x < rows && y < cols;
}

void Enemy::a_star(std::vector<std::vector<int>>& grid, Node start, Node goal)
{
	std::priority_queue<Node> open;
	std::unordered_map<int, std::unordered_map<int, bool>> closed;
	std::vector<std::vector<int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

	int rows = grid.size();
	int cols = grid[0].size();

	start.g = 0;
	start.h = heuristic(start.x, start.y, goal.x, goal.y);
	start.f = start.g + start.h;
	open.push(start);

	while (!open.empty()) {
		Node curr = open.top();
		open.pop();
		closed[curr.x][curr.y] = true;

		if (curr.x == goal.x && curr.y == goal.y) {
			std::cout << "Found a path with a cost of " << curr.g << std::endl;
			return;
		}

		for (const auto& dir : directions) {
			int x = curr.x + dir[0];
			int y = curr.y + dir[1];
			if (isValid(x, y, rows, cols) && grid[x][y] != 1 && !closed[x][y]) {
				Node neighbor;
				neighbor.x = x;
				neighbor.y = y;
				neighbor.g = curr.g + 1;
				neighbor.h = heuristic(x, y, goal.x, goal.y);
				neighbor.f = neighbor.g + neighbor.h;
				open.push(neighbor);
			}
		}
	}

	std::cout << "No path found\n";
}
