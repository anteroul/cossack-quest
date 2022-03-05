#include "raylib.h"
#include "Game.h"

int main()
{
    InitWindow(1280, 720, "The Wrath of Mictlantecuhtli");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    Game game;

    while (!WindowShouldClose())
    {
        game.runApplication();
    }

    game.deInit();
    CloseWindow();

    return 0;
}