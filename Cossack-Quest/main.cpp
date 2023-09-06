#include "Game.hpp"

int main()
{
    Game game(1280, 720, "The Last Cossack", true, true);

    while (!WindowShouldClose())
    {
        game.runApplication();
    }
    
    return 0;
}