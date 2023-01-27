#include <array>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>
#include "Player.hpp"
#include "PlayerControl.hpp"
#include "GameObject.hpp"

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
	int level = 0;
	Shader shader;
	Camera camera;
	Player player;
	GameObject* wall = nullptr;
	std::array<BoundingBox*, 64> walls = { nullptr };
	Texture2D hud;
	Texture2D weaponTexture[5];
	Vector2 weaponPosition;
	Rectangle weaponRec;

	struct weaponData {
		unsigned int cFrame = 0;
		unsigned int cWeapon = 0;
		int weaponFrame = 0;
	};

	weaponData wd;

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
