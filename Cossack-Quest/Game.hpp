#include <cstdlib>
#include <raylib.h>
#include <raymath.h>
#include "Player.hpp"
#include "GameManager.hpp"
#include "Enemy.hpp"

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
	bool noClip = false;
	bool debugMode = true;
	bool debugCoordinates = true;
	int ambientLoc;
	int fogDensityLoc;
	float fogDensity = 0.12f;
	unsigned int cFrame = 0;
	unsigned int cWeapon = 0;
	int weaponFrame = 0;
	int level = 0;
	Shader shader;
	GameObject* wall = nullptr;
	Player player;
	Camera camera;
	Texture2D hud;
	Texture2D weaponTexture[5];
	Vector2 weaponPosition;
	Rectangle weaponRec;
	BoundingBox* walls[64] = { nullptr };
    [[maybe_unused]] Sound swingSfx;

	int map[8][8] = {
			1, 1, 1, 1, 1, 1, 1, 1,
			1, 0, 0, 1, 0, 0, 0, 1,
			1, 0, 0, 1, 0, 1, 0, 1,
			1, 0, 0, 0, 0, 1, 0, 1,
			1, 1, 1, 0, 1, 1, 0, 1,
			1, 0, 1, 0, 1, 1, 1, 1,
			1, 0, 0, 0, 0, 0, 0, 1,
			1, 1, 1, 1, 1, 1, 1, 1
	};
};