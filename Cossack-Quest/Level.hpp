#pragma once

#include <algorithm>
#include <array>
#include <raylib.h>
#include <raymath.h>

// Collision boxes of the level's wall cells, null for open cells
using Walls = std::array<BoundingBox*, 64>;

inline bool isInsideWall(Vector3 point, const Walls& walls)
{
    return std::ranges::any_of(walls, [&](const BoundingBox* wall) {
        return wall != nullptr && point.x >= wall->min.x && point.x <= wall->max.x && point.y >= wall->min.y &&
               point.y <= wall->max.y && point.z >= wall->min.z && point.z <= wall->max.z;
    });
}

// True when no wall blocks the straight line between two points
inline bool hasLineOfSight(Vector3 from, Vector3 to, const Walls& walls)
{
    const Vector3 delta = Vector3Subtract(to, from);
    const float distance = Vector3Length(delta);
    if (distance < 0.001f) return true;

    const Ray ray = {from, Vector3Scale(delta, 1.0f / distance)};
    return std::ranges::none_of(walls, [&](const BoundingBox* wall) {
        if (wall == nullptr) return false;
        const RayCollision hit = GetRayCollisionBox(ray, *wall);
        return hit.hit && hit.distance < distance;
    });
}
