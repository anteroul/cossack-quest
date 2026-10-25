#pragma once

#include <algorithm>
#include <raylib.h>

// One row of a sprite sheet: frameCount equally sized frames, left to right
struct SpriteAnimation
{
    int row;
    int frameCount;
    float fps;
    bool loop;
};

// Plays SpriteAnimations and gives the source rectangle of the current frame.
// Animations are referenced, not copied, so they must outlive the Animator (they're static constants).
class Animator
{
public:
    // Switch to anim; keeps it running if it's already playing, unless restart is set
    void play(const SpriteAnimation& anim, bool restart = false)
    {
        if (animation == &anim && !restart) return;
        animation = &anim;
        time = 0.0f;
    }

    void update(float dt) { time += dt; }

    int frame() const
    {
        if (animation == nullptr) return 0;
        const int index = static_cast<int>(time * animation->fps);
        return animation->loop ? index % animation->frameCount : std::min(index, animation->frameCount - 1);
    }

    // A one-shot animation is finished once its last frame has been shown for a full frame
    bool finished() const
    {
        return animation != nullptr && !animation->loop && time * animation->fps >= animation->frameCount;
    }

    Rectangle sourceRect(float frameWidth, float frameHeight) const
    {
        const int row = animation != nullptr ? animation->row : 0;
        return {frame() * frameWidth, row * frameHeight, frameWidth, frameHeight};
    }

private:
    const SpriteAnimation* animation = nullptr;
    float time = 0.0f;
};
