#include "raylib.h"
#include "iostream"
#include "Game.h"

#define GLSL_VERSION  330

int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1280, 720, "Valiant Engine V2");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
    ToggleFullscreen();

    if (GLSL_VERSION != 330)
    {
        std::cout << "ERROR! Initializing OpenGL failed! Closing program.\n";
        CloseWindow();
        return 1;
    }

    else
    {
        std::cout << "Window created successfully.\n";
        std::cout << "Resolution: " << GetScreenWidth() << "x" << GetScreenHeight() << std::endl;
        std::cout << "Target Framerate: " << GetMonitorRefreshRate(GetCurrentMonitor()) << std::endl;
    }

    Game game;

    while (!WindowShouldClose())
    {
        game.runApplication();
    }

    game.deInit();
    CloseWindow();
    
    return 0;
}