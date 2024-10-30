#pragma once
#include <cmath>

enum State {
    IDLE,
    STUCK,
    PATROL,
    CHASE,
    ATTACK
};

static int convertToGrid(float n)
{
    return static_cast<int>(std::round(n * 8.0f / 16.0f));
}
