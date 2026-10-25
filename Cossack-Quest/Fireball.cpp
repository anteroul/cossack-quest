#include "Fireball.hpp"
#include <raymath.h>

namespace
{
constexpr float SPEED = 14.0f;     // World units per second
constexpr float MAX_AGE = 6.0f;    // Seconds before a fireball that hit nothing burns out
constexpr float HIT_RADIUS = 1.1f; // Horizontal distance from the player that counts as a hit

const SpriteAnimation FLIGHT = {0, 4, 12.0f, true};
const SpriteAnimation EXPLOSION = {1, 4, 14.0f, false};
} // namespace

Fireball::Fireball(Vector3 from, Vector3 target, int damage) : position(from), damage(damage)
{
    velocity = Vector3Scale(Vector3Normalize(Vector3Subtract(target, from)), SPEED);
    animator.play(FLIGHT);
}

void Fireball::update(float dt, Player& player, const Walls& walls)
{
    animator.update(dt);
    if (exploding) return;

    age += dt;
    position = Vector3Add(position, Vector3Scale(velocity, dt));

    const bool hitsPlayer = Vector2Distance({position.x, position.z}, {player.playerPos.x, player.playerPos.z}) <
                                HIT_RADIUS &&
                            position.y < player.playerPos.y + 0.5f;
    if (hitsPlayer && !player.gameOver) {
        player.takeDamage(damage);
        explode();
    } else if (isInsideWall(position, walls) || position.y < 0.0f || age > MAX_AGE) {
        explode();
    }
}

void Fireball::explode()
{
    exploding = true;
    animator.play(EXPLOSION);
}
