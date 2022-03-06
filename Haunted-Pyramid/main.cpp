#include "raylib.h"
#include "Game.h"

int main()
{
    InitWindow(1280, 720, "Valiant Engine V2");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
    //ToggleFullscreen();

    Game game;

    while (!WindowShouldClose())
    {
        game.runApplication();
    }

    game.deInit();
    CloseWindow();

    return 0;
}