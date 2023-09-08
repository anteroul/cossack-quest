#include <array>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>
#include "Player.hpp"
#include "PlayerControl.hpp"
#include "GameObject.hpp"
#include "Enemy.hpp"

class Game
{
public:
	Game(int wWidth, int wHeight, const char* wTitle, bool fullscreenEnabled, bool consoleEnabled);
	~Game();
	void runApplication();
private:
	void update();
	void initGame();
	void draw();
	void resetGame();
	bool noClip = false;
	bool debugMode = true;
	bool debugCoordinates = true;
	int ambientLoc{};
	int fogDensityLoc{};
	float fogDensity = 0.12f;
	int level = 0;
	Shader shader{};
	Camera3D cam3D{};
    Camera2D cam2D{};
	Player player;
    Enemy* enemy = nullptr;
	GameObject* wall = nullptr;
	std::array<BoundingBox*, 64> walls = { nullptr };
	Texture2D hud{};
	Texture2D weaponTexture[5]{};
	Vector2 weaponPosition{};
    Vector2 windowSize{};
	Rectangle weaponRec{};

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
