#include "NavGrid.hpp"
#include <cmath>
#include <queue>
#include <vector>

NavGrid::NavGrid(const int map[SIZE][SIZE])
{
    for (int x = 0; x < SIZE; x++)
        for (int y = 0; y < SIZE; y++) walkable[x][y] = map[x][y] == 0;

    for (auto& column : distance) column.fill(UNREACHABLE);
}

std::optional<Cell> NavGrid::worldToCell(Vector3 pos)
{
    const Cell cell = {static_cast<int>(std::floor((pos.x - ORIGIN) / CELL_SIZE)),
                       static_cast<int>(std::floor((pos.z - ORIGIN) / CELL_SIZE))};
    if (!inBounds(cell)) return std::nullopt;
    return cell;
}

Vector3 NavGrid::cellCenter(Cell cell, float y)
{
    return {ORIGIN + (cell.x + 0.5f) * CELL_SIZE, y, ORIGIN + (cell.y + 0.5f) * CELL_SIZE};
}

bool NavGrid::isWalkable(Cell cell) const
{
    return inBounds(cell) && walkable[cell.x][cell.y];
}

void NavGrid::computeDistances(Cell newGoal)
{
    if (goal == newGoal) return;
    goal = newGoal;

    for (auto& column : distance) column.fill(UNREACHABLE);
    if (!inBounds(newGoal)) return;

    using Node = std::pair<float, Cell>; // (cost so far, cell)
    auto cheaper = [](const Node& a, const Node& b) { return a.first > b.first; };
    std::priority_queue<Node, std::vector<Node>, decltype(cheaper)> open(cheaper);

    distance[newGoal.x][newGoal.y] = 0.0f;
    open.emplace(0.0f, newGoal);

    while (!open.empty()) {
        const auto [cost, cell] = open.top();
        open.pop();
        if (cost > distance[cell.x][cell.y]) continue; // A cheaper route to this cell was already settled

        forEachNeighbor(cell, [&](Cell next, float stepCost) {
            const float nextCost = cost + stepCost;
            if (nextCost < distance[next.x][next.y]) {
                distance[next.x][next.y] = nextCost;
                open.emplace(nextCost, next);
            }
        });
    }
}

float NavGrid::distanceToGoal(Cell cell) const
{
    return inBounds(cell) ? distance[cell.x][cell.y] : UNREACHABLE;
}

std::optional<Cell> NavGrid::nextStep(Cell from) const
{
    if (!goal || from == *goal) return std::nullopt;

    // Moves are symmetric, so the neighbor that minimizes (step cost + its distance) lies on a shortest path
    std::optional<Cell> best;
    float bestCost = UNREACHABLE;
    forEachNeighbor(from, [&](Cell next, float stepCost) {
        const float cost = stepCost + distance[next.x][next.y];
        if (cost < bestCost) {
            bestCost = cost;
            best = next;
        }
    });
    return best;
}
