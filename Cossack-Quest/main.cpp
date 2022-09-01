#include "raylib.h"
#include "Game.h"

#define GLSL_VERSION  330

int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1920, 1080, "Valiant Engine V2");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
    ToggleFullscreen();

    Game game;

    while (!WindowShouldClose())
    {
        game.runApplication();
    }

    game.deInit();
    CloseWindow();
    
    return 0;
}