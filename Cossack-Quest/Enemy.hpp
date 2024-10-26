#ifndef COSSACKQUEST_ENEMY_HPP
#define COSSACKQUEST_ENEMY_HPP

#include <raylib.h>
#include "GameObject.hpp"
#include "Player.hpp"
#include <array>
#include <iostream>
#include <vector>
#include <limits>
#include <queue>
#include <unordered_map>
#include "matrix.h"
#include "util.h"


#define ROWS 8
#define COLS 8

class Enemy : public GameObject {
    public:
        Enemy(Vector3 pos, Texture tex, Model mod, std::array<BoundingBox*, 64> wa);
        ~Enemy();
        void update(Player* target);
        BoundingBox getBounds();
        Vector3 position{};
        Texture texture{};
        Model model{};

    private:
        std::pair<int, int> dijkstra(int map[ROWS][COLS], int startRow, int startCol, int endRow, int endCol);
        const int INF = std::numeric_limits<int>::max(); // Define infinity
        std::array<BoundingBox*, 64> wallArray{};
        BoundingBox box{};
        State state;
        bool alive;
        bool attacking;
        float yaw;
        int playerX, playerY;
        int enemyX, enemyY;
        int attackTimer;
        int maxDamage;
};

#endif //COSSACKQUEST_ENEMY_HPP
