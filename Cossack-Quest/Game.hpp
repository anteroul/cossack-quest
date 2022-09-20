#include "cstdlib"
#include "raylib.h"
#include "raymath.h"
#include "Globals.hpp"
#include "GameManager.hpp"
#include "Levels.hpp"
#include "Player.hpp"

class Game
{
public:
	Game();
	~Game();
	void runApplication();
	void deInit();
private:
	void update();
	void initGame();
	void draw();
	void resetGame();
	void handleInput();
	void loadLevel(int level);
	bool debugMode = true;
	int ambientLoc;
	int fogDensityLoc;
	float fogDensity = 0.12f;
	Shader shader;
	BoundingBox* walls[64] = { nullptr };
    [[maybe_unused]] Sound swingSfx;
	Player player;
};