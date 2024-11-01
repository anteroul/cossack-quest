#ifndef COSSACKQUEST_ENEMY_HPP
#define COSSACKQUEST_ENEMY_HPP

#include "GameObject.hpp"
#include "Player.hpp"
#include <array>
#include <limits>
#include <queue>
#include <unordered_map>
#include "util.h"


#define ROWS 8
#define COLS 8

class Enemy : public GameObject {
    public:
        Enemy(Vector3 pos, Texture tex, Model mod, std::array<BoundingBox*, 64> wa);
        ~Enemy();
        void update(Player* target);
        Vector3 position{};
        Texture texture{};
        Model model{};
    private:
        void initializeMap(int map[8][8], Player* target);
        void moveToWaypoint(const std::pair<int, int>& waypoint);
        void adjustYawToPlayer(Player* target);
        static bool isPositionWithinCell(const Vector3& pos, int x, int y);
        bool checkCollisions(const BoundingBox& collider) const;
        static std::pair<int, int> dijkstra(int map[8][8], int startRow, int startCol, int endRow, int endCol);
        const int INF = std::numeric_limits<int>::max(); // Define infinity
        
        std::array<BoundingBox*, 64> wallArray{};
        BoundingBox box{};
        State state;
        std::pair<int, int> lastPosition;
        Vector3 positionBeforeCollision{};
        bool alive;
        bool attacking;
        float yaw;
        int playerX, playerY;
        int enemyX, enemyY;
        int attackTimer;
        int maxDamage;
};

#endif //COSSACKQUEST_ENEMY_HPP
