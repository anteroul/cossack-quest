#include "Enemy.hpp"
#include <algorithm>
#include <cmath>
#include <raymath.h>

namespace
{
constexpr float TURN_SPEED = 270.0f;    // Degrees per second
constexpr float AGGRO_DISTANCE = 4.0f;  // Path cost in cells at which an idle enemy notices the player
constexpr float HIT_FLASH_TIME = 0.12f; // Seconds
constexpr float BLOCK_TIME = 0.5f;      // Seconds the shield stays up after a block
constexpr float SHIELD_ARC = 70.0f;     // Degrees either side of straight ahead that the shield covers

float horizontalDistance(Vector3 a, Vector3 b)
{
    return Vector2Distance({a.x, a.z}, {b.x, b.z});
}

bool roll(float chance)
{
    return GetRandomValue(1, 100) <= static_cast<int>(chance * 100.0f);
}
} // namespace

// Sheet rows (see tools/generate_assets.py): 0 walk, 1 attack, 2 hit, 3 death, 4 block
const EnemyType WIZARD = {
    .kind = EnemyKind::Wizard,
    .maxHealth = 250,
    .speed = 3.0f,
    .spriteSize = 5.5f,
    .sightRange = 24.0f,
    .attackRange = 16.0f,
    .attackCooldown = 2.2f,
    .attackFrame = 2,
    .minDamage = 12,
    .maxDamage = 20,
    .ranged = true,
    .keepDistance = 10.0f,
    .painChance = 1.0f,
    .blockChance = 0.0f,
    .idle = {0, 1, 1.0f, true},
    .walk = {0, 4, 6.0f, true},
    .attack = {1, 4, 6.0f, false},
    .hit = {2, 2, 6.0f, false},
    .death = {3, 6, 8.0f, false},
    .block = {0, 1, 1.0f, true},
};

const EnemyType SKELETON_WARRIOR = {
    .kind = EnemyKind::SkeletonWarrior,
    .maxHealth = 250,
    .speed = 3.5f,
    .spriteSize = 5.5f,
    .sightRange = 24.0f,
    .attackRange = 5.0f,
    .attackCooldown = 1.0f,
    .attackFrame = 2,
    .minDamage = 15,
    .maxDamage = 25,
    .ranged = false,
    .keepDistance = 0.0f,
    .painChance = 0.25f,
    .blockChance = 0.5f,
    .idle = {0, 1, 1.0f, true},
    .walk = {0, 4, 7.0f, true},
    .attack = {1, 4, 7.0f, false},
    .hit = {2, 2, 6.0f, false},
    .death = {3, 6, 8.0f, false},
    .block = {4, 2, 2.0f / BLOCK_TIME, false},
};

Enemy::Enemy(const EnemyType& type, Vector3 spawn) : position(spawn), enemyType(&type), health(type.maxHealth)
{
    animator.play(type.idle);
}

void Enemy::update(float dt, Player& player, const NavGrid& nav, const Walls& walls, std::vector<Fireball>& fireballs)
{
    const EnemyType& t = *enemyType;
    animator.update(dt);
    hitFlash = std::max(0.0f, hitFlash - dt);
    attackCooldown = std::max(0.0f, attackCooldown - dt);

    if (state == State::Dead) return; // The death animation stops on its last frame, leaving a corpse
    if (player.gameOver) {
        state = State::Idle;
        animator.play(t.idle);
        return;
    }

    const float playerDistance = horizontalDistance(position, player.playerPos);
    const Vector3 eyes = {position.x, position.y + t.spriteSize * 0.8f, position.z};
    const bool canSee = playerDistance <= t.sightRange && hasLineOfSight(eyes, player.playerPos, walls);

    switch (state) {
    case State::Idle: {
        animator.play(t.idle);
        const auto cell = NavGrid::worldToCell(position);
        if (canSee || (cell && nav.distanceToGoal(*cell) <= AGGRO_DISTANCE)) state = State::Chase;
        break;
    }

    case State::Chase: {
        turnTowards(player.playerPos, dt);
        if (canSee && playerDistance <= t.attackRange && attackCooldown <= 0.0f) {
            startAttack(false);
            break;
        }

        // Wizards keep their distance while they have a clear shot; melee enemies stop at arm's length
        const bool holdPosition =
            t.ranged ? canSee && playerDistance <= t.keepDistance : playerDistance <= t.attackRange * 0.8f;
        if (holdPosition) {
            animator.play(t.idle);
        } else {
            animator.play(t.walk);
            chase(dt, player, nav, walls);
        }
        break;
    }

    case State::Attack:
        turnTowards(player.playerPos, dt);
        if (!attackDone && animator.frame() >= t.attackFrame) {
            attackDone = true;
            strike(player, playerDistance, fireballs);
        }
        if (animator.finished()) {
            attackCooldown = t.attackCooldown;
            state = State::Chase;
        }
        break;

    case State::Hurt:
        if (animator.finished()) state = State::Chase;
        break;

    case State::Block:
        turnTowards(player.playerPos, dt);
        blockTimer -= dt;
        if (blockTimer <= 0.0f) {
            // Counterattack as soon as the shield comes down
            if (canSee && playerDistance <= t.attackRange) startAttack(true);
            else state = State::Chase;
        }
        break;

    case State::Dead:
        break;
    }
}

void Enemy::startAttack(bool counter)
{
    state = State::Attack;
    attackDone = false;
    counterattack = counter;
    animator.play(enemyType->attack, true);
}

void Enemy::strike(Player& player, float playerDistance, std::vector<Fireball>& fireballs) const
{
    const EnemyType& t = *enemyType;
    const int damage = GetRandomValue(t.minDamage, t.maxDamage);

    if (t.ranged) {
        // Thrown from the raised hand towards the player's chest
        const Vector3 forward = {std::sin(yaw * DEG2RAD), 0.0f, std::cos(yaw * DEG2RAD)};
        const Vector3 hand = Vector3Add({position.x, position.y + t.spriteSize * 0.6f, position.z},
                                        Vector3Scale(forward, RADIUS + 0.3f));
        fireballs.emplace_back(hand, Vector3{player.playerPos.x, player.playerPos.y - 0.8f, player.playerPos.z},
                               damage);
    } else if (playerDistance <= t.attackRange * 1.2f) {
        player.takeDamage(damage); // Backing off during the wind-up dodges the blow
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

    const float step = std::min(enemyType->speed * dt, length);
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

bool Enemy::isFacing(Vector3 point) const
{
    const Vector2 toPoint = Vector2Normalize({point.x - position.x, point.z - position.z});
    const Vector2 forward = {std::sin(yaw * DEG2RAD), std::cos(yaw * DEG2RAD)};
    return Vector2DotProduct(toPoint, forward) >= std::cos(SHIELD_ARC * DEG2RAD);
}

Enemy::HitResult Enemy::takeDamage(int dmg, Vector3 attacker)
{
    if (state == State::Dead) return HitResult::Missed;
    const EnemyType& t = *enemyType;

    // The shield only covers the front. It can come up during an attack's wind-up (cancelling the attack),
    // but not once the swing is underway, during a counterattack, or while staggered. Once raised it blocks
    // every hit, but comes down after BLOCK_TIME however often it's hit, so it can't be pinned behind it.
    const bool swinging = state == State::Attack && (counterattack || animator.frame() >= t.attackFrame);
    const bool canBlock = t.blockChance > 0.0f && !swinging && state != State::Hurt && isFacing(attacker);
    if (canBlock && state == State::Block) return HitResult::Blocked;
    if (canBlock && roll(t.blockChance)) {
        state = State::Block;
        blockTimer = BLOCK_TIME;
        animator.play(t.block, true);
        return HitResult::Blocked;
    }

    health -= dmg;
    hitFlash = HIT_FLASH_TIME;
    if (health <= 0) {
        state = State::Dead;
        animator.play(t.death, true);
        return HitResult::Killed;
    }

    if (roll(t.painChance)) {
        state = State::Hurt;
        animator.play(t.hit, true);
    } else if (state == State::Idle) {
        state = State::Chase; // Getting hit always wakes the enemy up
    }
    return HitResult::Damaged;
}

BoundingBox Enemy::bounds() const
{
    const float height = enemyType->spriteSize * 0.9f;
    return {{position.x - RADIUS, position.y, position.z - RADIUS},
            {position.x + RADIUS, position.y + height, position.z + RADIUS}};
}

bool Enemy::collidesWithWall(Vector3 pos, const Walls& walls) const
{
    const float height = enemyType->spriteSize * 0.9f;
    const BoundingBox box = {{pos.x - RADIUS, pos.y, pos.z - RADIUS}, {pos.x + RADIUS, pos.y + height, pos.z + RADIUS}};
    return std::ranges::any_of(walls, [&](const BoundingBox* wall) {
        return wall != nullptr && CheckCollisionBoxes(box, *wall);
    });
}
