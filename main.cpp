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

	try {
		if (GLSL_VERSION == 330)
			std::cout << "Shaders loaded.\n";
	} catch (std::exception ex) {
        std::cout << "ERROR! Initializing OpenGL shaders failed! Closing program.\n";
        CloseWindow();
        return 1;
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