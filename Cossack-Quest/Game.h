#include "raylib.h"
#include "raymath.h"
#include "cstdlib"
#include "Globals.h"
#include "GameManager.h"

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
	bool debugMode = true;
	int ambientLoc;
	int fogDensityLoc;
	float fogDensity = 0.12f;
	Shader shader;
	BoundingBox* walls[64] = { nullptr };
	Sound swingSfx;
};