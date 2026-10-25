#include "Enemy.hpp"
#include <algorithm>
#include <cmath>
#include <raymath.h>

namespace
{
constexpr float SPEED = 3.5f;              // World units per second
constexpr float TURN_SPEED = 270.0f;       // Degrees per second
constexpr float AGGRO_DISTANCE = 4.0f;     // Path cost in cells at which an idle enemy notices the player
constexpr float ATTACK_RANGE = 3.5f;       // Horizontal distance to the player
constexpr float ATTACK_WINDUP = 0.4f;      // Seconds
constexpr float ATTACK_COOLDOWN = 1.2f;    // Seconds
constexpr int MIN_DAMAGE = 10;
constexpr int MAX_DAMAGE = 20;
constexpr float HIT_FLASH_TIME = 0.15f;    // Seconds
constexpr float SINK_SPEED = 2.0f;         // How fast a dead enemy sinks into the ground
constexpr float SINK_DEPTH = -Enemy::HEIGHT;

float horizontalDistance(Vector3 a, Vector3 b)
{
    return Vector2Distance({a.x, a.z}, {b.x, b.z});
}
} // namespace

Enemy::Enemy(Vector3 spawn) : position(spawn)
{}

void Enemy::update(float dt, Player& player, const NavGrid& nav, const Walls& walls)
{
    hitFlash = std::max(0.0f, hitFlash - dt);
    attackCooldown = std::max(0.0f, attackCooldown - dt);

    if (state == State::Dead) {
        position.y = std::max(SINK_DEPTH, position.y - SINK_SPEED * dt);
        return;
    }
    if (player.gameOver) {
        state = State::Idle;
        return;
    }

    const float playerDistance = horizontalDistance(position, player.playerPos);

    switch (state) {
    case State::Idle:
        if (const auto cell = NavGrid::worldToCell(position); cell && nav.distanceToGoal(*cell) <= AGGRO_DISTANCE)
            state = State::Chase;
        break;

    case State::Chase:
        if (playerDistance > ATTACK_RANGE) {
            chase(dt, player, nav, walls);
        } else if (attackCooldown <= 0.0f) {
            state = State::Attack;
            attackTimer = ATTACK_WINDUP;
        }
        turnTowards(player.playerPos, dt);
        break;

    case State::Attack:
        turnTowards(player.playerPos, dt);
        attackTimer -= dt;
        if (attackTimer <= 0.0f) {
            // The player can still dodge by backing off during the wind-up
            if (playerDistance <= ATTACK_RANGE) player.takeDamage(GetRandomValue(MIN_DAMAGE, MAX_DAMAGE));
            attackCooldown = ATTACK_COOLDOWN;
            state = State::Chase;
        }
        break;

    case State::Dead:
        break;
    }
}

void Enemy::chase(float dt, const Player& player, const NavGrid& nav, const Walls& walls)
{
    const auto cell = NavGrid::worldToCell(position);
    if (!cell) return;

    // In the player's cell head straight for them, otherwise walk to the center of the next cell on the path.
    // Each move stays inside the 2x2 block of open cells around the step, so it never crosses a wall corner.
    Vector3 target = player.playerPos;
    if (nav.goalCell() != cell) {
        const auto next = nav.nextStep(*cell);
        if (!next) return; // Player can't be reached from here
        target = NavGrid::cellCenter(*next, position.y);
    }

    const Vector2 toTarget = {target.x - position.x, target.z - position.z};
    const float length = Vector2Length(toTarget);
    if (length < 0.001f) return;

    const float step = std::min(SPEED * dt, length);
    move(toTarget.x / length * step, toTarget.y / length * step, walls);
}

void Enemy::move(float dx, float dz, const Walls& walls)
{
    if (const Vector3 next = {position.x + dx, position.y, position.z}; !collidesWithWall(next, walls))
        position = next;
    if (const Vector3 next = {position.x, position.y, position.z + dz}; !collidesWithWall(next, walls))
        position = next;
}

void Enemy::turnTowards(Vector3 target, float dt)
{
    const float targetYaw = std::atan2(target.x - position.x, target.z - position.z) * RAD2DEG;
    const float diff = std::remainder(targetYaw - yaw, 360.0f); // Shortest way round, in [-180, 180]
    const float maxTurn = TURN_SPEED * dt;
    yaw += std::clamp(diff, -maxTurn, maxTurn);
}

void Enemy::takeDamage(int dmg)
{
    if (state == State::Dead) return;

    health -= dmg;
    hitFlash = HIT_FLASH_TIME;
    if (health <= 0) state = State::Dead;
    else if (state == State::Idle) state = State::Chase; // Getting hit always wakes the enemy up
}

BoundingBox Enemy::bounds() const
{
    return {{position.x - RADIUS, position.y, position.z - RADIUS},
            {position.x + RADIUS, position.y + HEIGHT, position.z + RADIUS}};
}

bool Enemy::collidesWithWall(Vector3 pos, const Walls& walls) const
{
    const BoundingBox box = {{pos.x - RADIUS, pos.y, pos.z - RADIUS}, {pos.x + RADIUS, pos.y + HEIGHT, pos.z + RADIUS}};
    return std::ranges::any_of(walls, [&](const BoundingBox* wall) {
        return wall != nullptr && CheckCollisionBoxes(box, *wall);
    });
}
