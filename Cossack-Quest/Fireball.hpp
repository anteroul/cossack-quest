#pragma once

#include <raylib.h>
#include "Level.hpp"
#include "Player.hpp"
#include "SpriteAnimation.hpp"

// A wizard's fireball: flies straight at where the player was when it was thrown and bursts on impact
class Fireball
{
public:
    static constexpr float SIZE = 1.8f;        // Billboard size in world units
    static constexpr float FRAME_SIZE = 32.0f; // Frame size in fireball.png

    Fireball(Vector3 from, Vector3 target, int damage);

    void update(float dt, Player& player, const Walls& walls);
    bool isFinished() const { return exploding && animator.finished(); }
    Rectangle sourceRect() const { return animator.sourceRect(FRAME_SIZE, FRAME_SIZE); }
    Vector3 position{};

private:
    void explode();

    Vector3 velocity{};
    int damage;
    float age = 0.0f;
    bool exploding = false;
    Animator animator;
};
