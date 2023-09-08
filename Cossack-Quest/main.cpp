#include "Game.hpp"

int main()
{
    Game game(1280, 720, "The Last Cossack", false, true);

    while (!WindowShouldClose())
    {
        game.runApplication();
    }
    
    return 0;
}