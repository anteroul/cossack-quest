#pragma once

#include <array>
#include <limits>
#include <optional>
#include <raylib.h>

struct Cell
{
    int x;
    int y;
    bool operator==(const Cell&) const = default;
};

// Walkability grid over the level map with Dijkstra path finding.
// Cell (x, y) is map[x][y] and covers world X/Z in [ORIGIN + x * CELL_SIZE, ORIGIN + (x + 1) * CELL_SIZE).
class NavGrid
{
public:
    static constexpr int SIZE = 8;
    static constexpr float CELL_SIZE = 8.0f;
    static constexpr float ORIGIN = -12.0f;
    static constexpr float UNREACHABLE = std::numeric_limits<float>::infinity();

    NavGrid() = default;
    explicit NavGrid(const int map[SIZE][SIZE]);

    static std::optional<Cell> worldToCell(Vector3 pos);
    static Vector3 cellCenter(Cell cell, float y);
    bool isWalkable(Cell cell) const;

    // Run Dijkstra outward from goal, storing the shortest path cost from every cell to it.
    // Does nothing if the goal hasn't changed since the last call.
    void computeDistances(Cell newGoal);
    std::optional<Cell> goalCell() const { return goal; }
    float distanceToGoal(Cell cell) const;
    // Next cell on the shortest path from `from` to the goal, or nullopt at the goal or if it can't be reached
    std::optional<Cell> nextStep(Cell from) const;

private:
    static bool inBounds(Cell cell) { return cell.x >= 0 && cell.x < SIZE && cell.y >= 0 && cell.y < SIZE; }

    // Calls visit(neighbor, cost) for every cell reachable in one move from cell. Moves are 8-directional;
    // diagonals cost sqrt(2) and are only allowed when both cells beside them are open, so paths don't cut corners.
    template <typename Visit>
    void forEachNeighbor(Cell cell, Visit visit) const
    {
        constexpr float DIAGONAL_COST = 1.41421356f;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                const Cell next = {cell.x + dx, cell.y + dy};
                if ((dx == 0 && dy == 0) || !isWalkable(next)) continue;

                if (dx != 0 && dy != 0) {
                    if (!isWalkable({cell.x + dx, cell.y}) || !isWalkable({cell.x, cell.y + dy})) continue;
                    visit(next, DIAGONAL_COST);
                } else {
                    visit(next, 1.0f);
                }
            }
        }
    }

    std::array<std::array<bool, SIZE>, SIZE> walkable{};
    std::array<std::array<float, SIZE>, SIZE> distance{};
    std::optional<Cell> goal;
};
