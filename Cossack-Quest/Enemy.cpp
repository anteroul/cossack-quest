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
    state = GUARD;
    attackTimer = 0;
    maxDamage = 25;
    box = {};
    playerX = 0;
    playerY = 0;
    enemyX = 0;
    enemyY = 0;
}

Enemy::~Enemy() = default;

void Enemy::update(Player* target)
{
    const int attackInterval = GetMonitorRefreshRate(GetCurrentMonitor()) * 3;
    const int minDamage = maxDamage / 2;
    Vector2 direction = { position.x - target->playerPos.x, position.z - target->playerPos.z };
    float angle = atan2(direction.y, direction.x) * RAD2DEG;
    bool collision = false;
    std::pair<int, int> waypoint;
    box = { {position.x - 3.5f, 0.f, position.z - 3.5f}, {position.x + 3.5f, 8.f, position.z + 3.5f} };
    BoundingBox collider = { {position.x - 1.5f, 0.f, position.z - 1.5f}, {position.x + 1.5f, 8.f, position.z + 1.5f} };

    // Define map grid:
    int map[8][8]{};
    int iterator = 0;

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            if (wallArray[iterator] != nullptr)
            {
                map[x][y] = 1;
            } else {
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
			iterator++;
        }
    }

	if (alive)
	{
        for (auto& i : wallArray)
        {
            if (i != nullptr)
            {
                if (CheckCollisionBoxes(collider, *i))
                    collision = true;
            }
        }

        if (target->health > 0)
        {
            switch (state)
            {
                case GUARD:
                    waypoint = dijkstra(map, enemyX, enemyY, playerX, playerY);

                    if (playerY == enemyY || playerX == enemyX)
                    {
                        state = CHASE;
                    }
                    if (waypoint.first > enemyX)
                        position.x += SPEED;
                    if (waypoint.first < enemyX)
                        position.x -= SPEED;
                    if (waypoint.second > enemyY)
                        position.z += SPEED;
                    if (waypoint.second < enemyY)
                        position.z -= SPEED;
                    break;

                case CHASE:
                    if (CheckCollisionBoxes(box, { target->playerPos, target->playerPos }))
                    {
                        if (attackTimer > attackInterval)
                            state = ATTACK;
                    }
                    
                    if (!collision)
                    {
                        if (target->playerPos.x > position.x)
                            position.x += SPEED;
                        if (target->playerPos.x < position.x)
                            position.x -= SPEED;
                        if (target->playerPos.z < position.z)
                            position.z -= SPEED;
                        if (target->playerPos.z > position.z)
                            position.z += SPEED;
                    } else {
                        state = GUARD;
                    }
                    attackTimer++;
                    break;

                case ATTACK:
                    attackTimer = 0;
                    target->takeDamage(rand() % maxDamage + minDamage);
                    state = CHASE;
                    break;

                default:
                    break;
            }
            std::cout << "State: " << state << std::endl;
            if (collision)
                std::cout << "Collision!\n";
        }
        if (angle > yaw + 90)
            yaw += 1.5f;
        if (angle < yaw + 90)
            yaw -= 1.5f;

        model.transform = MatrixRotateXYZ({0.0f, DEG2RAD*yaw*(-1), 0.0f});
	}
}

std::pair<int, int> Enemy::dijkstra(int map[ROWS][COLS], int startRow, int startCol, int endRow, int endCol)
{
    const int dr[] = { 1, -1, 0, 0 };
    const int dc[] = { 0, 0, -1, 1 };

    std::vector<std::vector<int>> distance(ROWS, std::vector<int>(COLS, INF));
    std::vector<std::vector<bool>> visited(ROWS, std::vector<bool>(COLS, false));

    distance[startRow][startCol] = 0;

    std::cout << "Enemy position: " << startRow << ", " << startCol << std::endl;

    while (true) {
        int minDist = INF;
        int u = -1, v = -1;

        for (int r = ROWS - 1; r > 0; --r)
        {
            for (int c = COLS - 1; c > 0; --c)
            {
                if (!visited[r][c] && distance[r][c] < minDist)
                {
                    minDist = distance[r][c];
                    u = r;
                    v = c;
                }
            }
        }

        if (u == -1 || v == -1 || (u == endRow && v == endCol))
            break;

        visited[u][v] = true;

        for (int dir = 0; dir < 4; ++dir) {
            int nr = u + dr[dir];
            int nc = v + dc[dir];

            if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && !visited[nr][nc] && distance[u][v] + map[nr][nc] < distance[nr][nc])
            {
                distance[nr][nc] = distance[u][v] + map[nr][nc];
            }
        }
    }

    // Find the neighbor with the smallest distance
    int minNeighborDist = INF;
    int minNeighborRow = -1, minNeighborCol = -1;

    for (int dir = 0; dir < 4; ++dir)
    {
        int nr = startRow + dr[dir];
        int nc = startCol + dc[dir];

        if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && distance[nr][nc] < minNeighborDist)
        {
            minNeighborDist = distance[nr][nc];
            minNeighborRow = nr;
            minNeighborCol = nc;
        }
    }

    std::cout << "Waypoint: " << minNeighborRow << ", " << minNeighborCol << std::endl;
    return std::make_pair(minNeighborRow, minNeighborCol);
}

BoundingBox Enemy::getBounds()
{
    return box;
}