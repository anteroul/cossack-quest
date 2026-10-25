#pragma once

#include <array>
#include <cstdlib>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include "Player.hpp"
#include "PlayerControl.hpp"
#include "GameObject.hpp"
#include "Enemy.hpp"
#include "NavGrid.hpp"
#include "Ui.hpp"

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
	void handlePlayerControls();
	void spawnEnemies();
	void updateEnemies();
	void playerAttack();
	bool noClip = false;
	bool debugMode = true;
	bool debugCoordinates = true;
	int ambientLoc{};
	int fogDensityLoc{};
	float fogDensity = 0.12f;
	int level = 0;
	int timer = 0;
	Shader shader{};
	Camera3D cam3D{};
	Player player{};
	GameObject* wall = nullptr;
	GameObject* ground = nullptr;
	GameObject* enemyAsset = nullptr; // Model and texture shared by every enemy
	std::vector<Enemy> enemies;
	NavGrid nav;
	static constexpr std::array<Cell, 2> ENEMY_SPAWNS = {Cell{6, 6}, Cell{1, 6}};
	static constexpr float ENEMY_MODEL_SCALE = 0.02f;
	static constexpr float ENEMY_MODEL_Y = 3.8f;      // Lifts the model so its feet touch the ground
	static constexpr float ENEMY_MODEL_YAW = 0.0f;    // Rotates the model so it faces the way it walks
	Texture2D enemySkinTexture{};  // Pure black, for the face and shoes
	Texture2D enemyFlashTexture{}; // Swapped onto the robe while an enemy flashes from a hit
	int enemyRobeMaterial = -1;
	static constexpr float MELEE_RANGE = 6.0f;
	std::array<BoundingBox*, 64> walls = { nullptr };
	Texture2D hud{};
	Texture2D weaponTexture[5]{};
	Texture2D crosshair{};
	Ui ui;
	// Weapon sprite layout in UI reference pixels:
	static constexpr float WEAPON_HUD_OVERLAP = 50.0f; // How far the sprite frame bottom tucks behind the HUD
	static constexpr float WEAPON_OFFSET_X = 24.0f;    // Shift left so the weapon art lines up with the crosshair
	float weaponOffsetY = 0.0f; // Extra downward offset (sword idle / swing)
	// HUD layout in hud.png pixels; the bar spans the full UI width and is scaled uniformly to fit:
	static constexpr float HUD_VALUE_Y = 72.0f;          // Top of the value text, below the labels
	static constexpr float HUD_VALUE_FONT_SIZE = 100.0f;
	static constexpr float HUD_NAME_FONT_SIZE = 38.0f;
	static constexpr float HUD_STAMINA_X = 237.0f;       // Horizontal centers of the labels in the artwork
	static constexpr float HUD_HEALTH_X = 540.0f;
	static constexpr float HUD_AMMO_X = 1075.0f;
	static constexpr float HUD_WEAPON_X = 1375.0f;
	Rectangle weaponRec{};
	Music music{};
	Sound deathSfx{};
	Sound slashSfx{};
	Sound gunshotSfx{};
	Sound crossbowSfx{};

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

	/*
	int map[8][8] = {
			1, 1, 1, 1, 1, 1, 1, 1,
			1, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 1, 0, 1, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 1, 0, 1, 1, 0, 1,
			1, 0, 1, 0, 1, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 1,
			1, 1, 1, 1, 1, 1, 1, 1
	};
	*/
};
