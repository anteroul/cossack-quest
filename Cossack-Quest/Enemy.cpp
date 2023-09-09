#include "Enemy.hpp"
#include <cmath>
#include <raymath.h>

#define SPEED (0.04)

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
                    playerX = x;
                    playerY = y;
                } else if (position.x > x * 8.0f - 12.0f && position.x < x * 8.0f - 4.0f && position.z > y * 8.0f - 12.0f && position.z < y * 8.0f - 4.0f) {
                    map[x][y] = 3;
                    enemyX = x;
                    enemyY = y;
                } else {
                    map[x][y] = 0;
                }
            }
			if (map[x][y] == 2)
                std::cout << "P,";
			else if (map[x][y] == 3)
				std::cout << "E,";
            else
                std::cout << map[x][y] << ",";
			iterator++;
        }
        std::cout << "\n";
    }

    std::cout << "\n";

	if (alive)
	{
        if (target->health > 0)
        {
            std::vector<std::vector<int>> distance(ROWS, std::vector<int>(COLS, INF));
            //distance = dijkstra(map, enemyX, enemyY, playerX, playerY, distance);
            //std::vector<std::pair<int, int>> path = backtrack(enemyX, enemyY, playerX, playerY, distance);
            for (auto& i : dijkstra(map, enemyX, enemyY, playerX, playerY, distance))
            {
                for (auto& j : i)
                {
                    // TODO
                }
            }
        }
        if (angle > yaw + 90)
            yaw += 0.5f;
        if (angle < yaw + 90)
            yaw -= 0.5f;

        model.transform = MatrixRotateXYZ({0.0f, DEG2RAD*yaw*(-1), 0.0f});
	}
}

std::vector<std::vector<int>> Enemy::dijkstra(int map[ROWS][COLS], int startRow, int startCol, int endRow, int endCol, std::vector<std::vector<int>>& distance)
{
    const int dr[] = { -1, 1, 0, 0 };
    const int dc[] = { 0, 0, -1, 1 };

    std::vector<std::vector<bool>> visited(ROWS, std::vector<bool>(COLS, false));

    distance[startRow][startCol] = 0;

    for (int i = 0; i < ROWS * COLS; ++i)
    {
        int minDist = INF;
        int u = -1, v = -1;

        for (int r = 0; r < ROWS; ++r)
        {
            for (int c = 0; c < COLS; ++c)
            {
                if (!visited[r][c] && distance[r][c] < minDist)
                {
                    minDist = distance[r][c];
                    u = r;
                    v = c;
                }
            }
        }

        if (u == -1 || v == -1) break;

        visited[u][v] = true;

        for (int dir = 0; dir < 4; ++dir)
        {
            int nr = u + dr[dir];
            int nc = v + dc[dir];

            if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && !visited[nr][nc] && distance[u][v] + map[nr][nc] < distance[nr][nc])
            {
                distance[nr][nc] = distance[u][v] + map[nr][nc];
            }
        }
    }

    if (distance[endRow][endCol] == INF)
    {
        std::cout << "No path found from (" << startRow << "," << startCol << ") to (" << endRow << "," << endCol << ")" << std::endl;
    } else {
        std::cout << "Shortest distance from (" << startRow << "," << startCol << ") to (" << endRow << "," << endCol << ") is: " << distance[endRow][endCol] << std::endl;
    }

    return distance;
}

// TODO: Lighter solution needed. This function is a memory hog!
std::vector<std::pair<int, int>> Enemy::backtrack(int startRow, int startCol, int endRow, int endCol, const std::vector<std::vector<int>>& distance)
{
    const int dr[] = { -1, 1, 0, 0 };
    const int dc[] = { 0, 0, -1, 1 };

    std::vector<std::pair<int, int>> path;
    
    int currentRow = endRow;
    int currentCol = endCol;

    while (currentRow != startRow || currentCol != startCol) {
        path.push_back({ currentRow, currentCol });

        for (int dir = 0; dir < 4; ++dir) {
            int nr = currentRow + dr[dir];
            int nc = currentCol + dc[dir];

            if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && distance[nr][nc] == distance[currentRow][currentCol] - 1) {
                currentRow = nr;
                currentCol = nc;
                break;
            }
        }
    }

    path.push_back({ startRow, startCol });
    std::reverse(path.begin(), path.end());

    return path;
}
