#include "Enemy.hpp"
#include <raymath.h>
#include <cmath>
#include <raymath.h>
#include <queue>
#include <limits>

#define SPEED 0.04f
#define INF std::numeric_limits<int>::max()

Enemy::Enemy(Vector3 pos, Texture tex, Model mod, std::array<BoundingBox*, 64> wa) : GameObject(pos, tex, mod)
{
    position = pos;
    texture = tex;
    model = mod;
    wallArray = wa;
    attacking = false;
    alive = true;
    yaw = 0.0f;
    state = PATROL;
    attackTimer = 0;
    maxDamage = 25;
    box = { {position.x - 1.5f, 0.f, position.z - 1.5f}, {position.x + 1.5f, 8.f, position.z + 1.5f} };
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

    // Define a clear map grid, resetting it each frame
    int map[8][8] = {};

    // Populate the grid with walls, enemy, and player positions
    initializeMap(map, target);

    if (alive) {
        BoundingBox collider = { {position.x - 0.5f, 0.f, position.z - 0.5f}, {position.x + 0.5f, 8.f, position.z + 0.5f} };
        
        if (!checkCollisions(collider))
            positionBeforeCollision = position;

        // Handle behavior states
        if (target->health > 0)
        {
            std::pair<int, int> waypoint = dijkstra(map, enemyX, enemyY, playerX, playerY);

            switch (state) {
            case STUCK:
                if (checkCollisions(collider))
                {
                    if (positionBeforeCollision.x < position.x) position.x -= SPEED;
                    else if (positionBeforeCollision.x > position.x) position.x += SPEED;
                    else if (positionBeforeCollision.z < position.z) position.z -= SPEED;
                    else if (positionBeforeCollision.z > position.z) position.z += SPEED;
                } else {
                    state = PATROL;
                }
                break;
            case PATROL:
                // Move towards waypoint and switch to CHASE if player is close enough
                moveToWaypoint(waypoint);
                if (std::abs(playerX - enemyX) <= 1 && std::abs(playerY - enemyY) <= 1) {
                    state = CHASE;
                } else if (checkCollisions(collider)) {
                    state = STUCK;
                }
                break;
            case CHASE:
                if (CheckCollisionBoxes(box, { target->playerPos, target->playerPos })) {
                    if (attackTimer > attackInterval) {
                        state = ATTACK;
                    }
                } else if (!checkCollisions(collider)) {
                    moveToWaypoint(waypoint);
                } else {
                    state = STUCK;
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
        }

        // Smoothly adjust yaw angle towards player
        adjustYawToPlayer(target);
    }
}

void Enemy::initializeMap(int map[8][8], Player* target)
{
    int iterator = 0;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (wallArray[iterator] != nullptr) {
                map[y][x] = INF; // Mark as obstacle
            }
            else {
                map[y][x] = 0;  // Walkable
            }

            if (isPositionWithinCell(target->playerPos, x, y)) {
                map[y][x] = 2;
                playerX = x;
                playerY = y;
            }
            else if (isPositionWithinCell(position, x, y)) {
                map[y][x] = 3;
                enemyX = x;
                enemyY = y;
            }

            iterator++;
        }
    }
}

bool Enemy::isPositionWithinCell(const Vector3& pos, int x, int y)
{
    return pos.x > x * 8.0f - 12.0f && pos.x < x * 8.0f - 4.0f && pos.z > y * 8.0f - 12.0f && pos.z < y * 8.0f - 4.0f;
}

bool Enemy::checkCollisions(const BoundingBox& collider)
{
    for (auto& wall : wallArray)
    {
        if (wall != nullptr && CheckCollisionBoxes(collider, *wall))
            return true;
    }
    return false;
}

void Enemy::moveToWaypoint(const std::pair<int, int>& waypoint)
{
    if (waypoint.first > enemyX) position.x += SPEED;
    if (waypoint.first < enemyX) position.x -= SPEED;
    if (waypoint.second > enemyY) position.z += SPEED;
    if (waypoint.second < enemyY) position.z -= SPEED;
}

void Enemy::adjustYawToPlayer(Player* target)
{
    const Vector2 direction = { position.x - target->playerPos.x, position.z - target->playerPos.z };
    float angle = atan2(direction.y, direction.x) * RAD2DEG;

    if (angle > yaw - 90) yaw += 1.5f;
    if (angle < yaw - 90) yaw -= 1.5f;

    model.transform = MatrixRotateXYZ({DEG2RAD * 90.f, 0.f, DEG2RAD * yaw});
}

std::pair<int, int> Enemy::dijkstra(int map[ROWS][COLS], int startRow, int startCol, int endRow, int endCol)
{
    const int dr[] = { 1, -1, 0, 0 }; // row directions (down, up)
    const int dc[] = { 0, 0, 1, -1 }; // column directions (right, left)

    std::vector<std::vector<int>> distance(ROWS, std::vector<int>(COLS, INF));
    std::vector<std::vector<std::pair<int, int>>> previous(ROWS, std::vector<std::pair<int, int>>(COLS, { -1, -1 }));

    distance[startRow][startCol] = 0;

    using Node = std::tuple<int, int, int>; // (distance, row, col)
    std::priority_queue<Node, std::vector<Node>, std::greater<>> pq;
    pq.emplace(0, startRow, startCol);

    while (!pq.empty()) {
        auto [dist, r, c] = pq.top();
        pq.pop();

        if (r == endRow && c == endCol) break; // Stop if we reached the destination

        for (int dir = 0; dir < 4; ++dir) {
            int nr = r + dr[dir];
            int nc = c + dc[dir];

            // Check bounds and walkability
            if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && map[nr][nc] != 1) {
                int newDist = dist + 1;
                if (newDist < distance[nr][nc]) {
                    distance[nr][nc] = newDist;
                    previous[nr][nc] = { r, c }; // Save path
                    pq.emplace(newDist, nr, nc);
                }
            }
        }
    }

    // Find the immediate next step from start to end
    int cr = endRow, cc = endCol;
    while (previous[cr][cc] != std::make_pair(startRow, startCol) && previous[cr][cc] != std::make_pair(-1, -1)) {
        auto [pr, pc] = previous[cr][cc];
        cr = pr;
        cc = pc;
    }

    return { cr, cc }; // Return the next cell to move toward the player
}