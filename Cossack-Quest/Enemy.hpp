#pragma once

#include <array>
#include <raylib.h>
#include "NavGrid.hpp"
#include "Player.hpp"

// A melee enemy that idles until the player comes within reach (by path distance, so walls count),
// then follows the shortest path from the NavGrid to the player and attacks when close.
// Enemies only hold state; the shared model is owned and drawn by Game.
class Enemy
{
public:
    using Walls = std::array<BoundingBox*, 64>;

    static constexpr float RADIUS = 1.0f;
    static constexpr float HEIGHT = 7.0f;
    static constexpr int MAX_HEALTH = 250;

    explicit Enemy(Vector3 spawn);

    void update(float dt, Player& player, const NavGrid& nav, const Walls& walls);
    void takeDamage(int dmg);
    // Move by (dx, dz) one axis at a time, dropping any part of the move that would enter a wall
    void move(float dx, float dz, const Walls& walls);

    bool isAlive() const { return state != State::Dead; }
    bool isHitFlashing() const { return hitFlash > 0.0f; }
    BoundingBox bounds() const;
    Vector3 position{};
    float yaw = 0.0f; // Degrees around the Y axis, 0 faces +Z

private:
    enum class State { Idle, Chase, Attack, Dead };

    void chase(float dt, const Player& player, const NavGrid& nav, const Walls& walls);
    void turnTowards(Vector3 target, float dt);
    bool collidesWithWall(Vector3 pos, const Walls& walls) const;

    State state = State::Idle;
    int health = MAX_HEALTH;
    float attackTimer = 0.0f;    // Wind-up left before the current attack lands
    float attackCooldown = 0.0f; // Time until the next attack can start
    float hitFlash = 0.0f;
};
