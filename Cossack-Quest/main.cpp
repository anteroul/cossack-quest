#include "Game.hpp"

int main()
{
    Game game(1280, 720, "The Chosen Cossack", false, false);

    while (!WindowShouldClose())
    {
        game.runApplication();
    }
    
    return 0;
}