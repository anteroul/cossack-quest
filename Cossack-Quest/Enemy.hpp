#pragma once

#include <vector>
#include <raylib.h>
#include "Fireball.hpp"
#include "Level.hpp"
#include "NavGrid.hpp"
#include "Player.hpp"
#include "SpriteAnimation.hpp"

enum class EnemyKind { Wizard, SkeletonWarrior, Count };

// Stats, behaviour and sprite sheet layout shared by every enemy of one kind
struct EnemyType
{
    EnemyKind kind;
    int maxHealth;
    float speed;          // World units per second
    float spriteSize;     // Billboard height and width in world units
    float sightRange;     // Notices the player within this distance when nothing blocks the view
    float attackRange;    // Horizontal distance to the player
    float attackCooldown; // Seconds between attacks
    int attackFrame;      // Frame of the attack animation where the blow lands or the fireball is thrown
    int minDamage;
    int maxDamage;
    bool ranged;          // Throws fireballs instead of fighting in melee
    float keepDistance;   // A ranged enemy stops approaching once this close with a clear shot
    float painChance;     // Chance that taking damage staggers the enemy, interrupting its attack
    float blockChance;    // Chance to block a hit from the front with a shield
    SpriteAnimation idle, walk, attack, hit, death, block;
};

extern const EnemyType WIZARD;
extern const EnemyType SKELETON_WARRIOR;

// An enemy that waits until it sees the player or the player comes near (by path distance), then follows
// the shortest path from the NavGrid and attacks. Drawn by Game as a camera-facing sprite.
class Enemy
{
public:
    enum class HitResult { Damaged, Blocked, Killed, Missed };

    static constexpr float RADIUS = 1.0f;
    static constexpr float FRAME_SIZE = 64.0f; // Frame size in the enemy sprite sheets

    Enemy(const EnemyType& type, Vector3 spawn);

    void update(float dt, Player& player, const NavGrid& nav, const Walls& walls, std::vector<Fireball>& fireballs);
    // Hit by the player standing at attacker. A shield bearer facing the attacker may block it.
    HitResult takeDamage(int dmg, Vector3 attacker);
    // Move by (dx, dz) one axis at a time, dropping any part of the move that would enter a wall
    void move(float dx, float dz, const Walls& walls);

    const EnemyType& type() const { return *enemyType; }
    bool isAlive() const { return state != State::Dead; }
    bool isHitFlashing() const { return hitFlash > 0.0f; }
    BoundingBox bounds() const;
    Rectangle sourceRect() const { return animator.sourceRect(FRAME_SIZE, FRAME_SIZE); }
    Vector3 position{};
    float yaw = 0.0f; // Degrees around the Y axis, 0 faces +Z

private:
    enum class State { Idle, Chase, Attack, Hurt, Block, Dead };

    void chase(float dt, const Player& player, const NavGrid& nav, const Walls& walls);
    void startAttack(bool counter);
    void strike(Player& player, float playerDistance, std::vector<Fireball>& fireballs) const;
    void turnTowards(Vector3 target, float dt);
    bool isFacing(Vector3 point) const;
    bool collidesWithWall(Vector3 pos, const Walls& walls) const;

    const EnemyType* enemyType;
    State state = State::Idle;
    int health;
    float attackCooldown = 0.0f; // Time until the next attack can start
    float blockTimer = 0.0f;     // Time left holding the shield up
    float hitFlash = 0.0f;
    bool attackDone = false;     // The current attack has already landed or thrown its fireball
    bool counterattack = false;  // The current attack follows a block and can't be cancelled by blocking
    Animator animator;
};
