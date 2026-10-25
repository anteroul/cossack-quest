#include "Game.hpp"

#define GLSL_VERSION  330

#define RLIGHTS_IMPLEMENTATION
#include "shaders/rlights.h"

Game::Game(int wWidth, int wHeight, const char* wTitle, bool fullscreenEnabled, bool consoleEnabled)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(wWidth, wHeight, wTitle);
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
    if (fullscreenEnabled) ToggleFullscreen();
    debugMode = consoleEnabled;
    InitAudioDevice();
    initGame();
}

Game::~Game()
{
    UnloadMusicStream(music);
    UnloadSound(slashSfx);
    UnloadSound(crossbowSfx);
    UnloadSound(gunshotSfx);
    UnloadSound(deathSfx);
    UnloadSound(blockSfx);
    CloseAudioDevice();

    for (auto& i : weaponTexture) UnloadTexture(i);

    UnloadTexture(hud);
    UnloadTexture(crosshair);
    UnloadShader(shader);

    for (auto& i : walls) delete i;

    delete wall;
    for (auto& sheet : enemySheets) UnloadTexture(sheet);
    UnloadTexture(fireballSheet);
    UnloadShader(spriteShader);
    delete ground;

    CloseWindow();
}

void Game::initGame()
{
    // Initialize textures and models:
    ground = new GameObject({0.0f, 0.0f, 0.0f}, LoadTexture("assets/dirt.png"),
                            LoadModelFromMesh(GenMeshPlane(8.0f * 16, 8.0f * 16, 8, 8)));
    ground->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = ground->texture;
    wall = new GameObject({0.0f, 0.0f, 0.0f}, LoadTexture("assets/wall.png"),
                          LoadModelFromMesh(GenMeshCube(8.0f, 8.0f, 8.0f)));
    wall->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = wall->texture;
    hud = LoadTexture("assets/hud.png");
    crosshair = LoadTexture("assets/crosshair.png");

    // Define weapon textures:
    weaponTexture[0] = LoadTexture("assets/weapons/weapon1.png");
    weaponTexture[1] = LoadTexture("assets/weapons/weapon2.png");
    weaponTexture[2] = LoadTexture("assets/weapons/weapon3.png");
    weaponTexture[3] = LoadTexture("assets/weapons/weapon4.png");
    weaponTexture[4] = LoadTexture("assets/weapons/weapon5.png");

    level = 1;

    player.stamina = 100;
    player.health = 100;
    player.gold = 0;
    player.gameOver = false;
    player.playerPos = {0.0f, 4.0f, 0.0f};
    player.weapon = weapon[wd.cWeapon];
    player.attacking = false;

    weaponOffsetY = 0.0f;
    weaponRec = {0, 0, (float)weaponTexture[0].width / 4, (float)weaponTexture[0].height / 2};

    // Define walls:
    int iterator = 0;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (map[x][y] == 1) walls[iterator] = new BoundingBox{
                x * 8.0f - 12.0f, 0.0f, y * 8.0f - 12.0f, x * 8.0f - 4.0f, 8.0f, y * 8.0f - 4.0f
            };
            else walls[iterator] = nullptr;
            iterator++;
        }
    }

    // Initialize shaders:
    shader = LoadShader("shaders/glsl/base_lighting.vs", "shaders/glsl/fog.fs");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    ambientLoc = GetShaderLocation(shader, "ambient");
    fogDensityLoc = GetShaderLocation(shader, "fogDensity");
    SetShaderValue(shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);

    // Define cam3D:
    cam3D.position = player.playerPos;
    cam3D.target = Vector3Add(player.playerPos, Vector3{0, 0, 1});
    cam3D.up = {0.0f, 1.0f, 0.0f};
    cam3D.fovy = 60.0f;
    cam3D.projection = CAMERA_PERSPECTIVE;

    // Using just 1 point lights
    CreateLight(LIGHT_DIRECTIONAL, {-100, 10, -100}, Vector3Zero(), DARKGRAY, shader);
    CreateLight(LIGHT_DIRECTIONAL, {-100, 10, 100}, Vector3Zero(), DARKGRAY, shader);
    CreateLight(LIGHT_DIRECTIONAL, {100, 10, 100}, Vector3Zero(), DARKGRAY, shader);
    CreateLight(LIGHT_DIRECTIONAL, {100, 10, -100}, Vector3Zero(), DARKGRAY, shader);

    // Apply shaders for 3D game objects
    wall->model.materials[0].shader = shader;

    // Initialize enemies (sprite sheets are made by tools/generate_assets.py):
    enemySheets[static_cast<size_t>(EnemyKind::Wizard)] = LoadTexture("assets/enemies/wizard.png");
    enemySheets[static_cast<size_t>(EnemyKind::SkeletonWarrior)] = LoadTexture("assets/enemies/skeleton.png");
    fireballSheet = LoadTexture("assets/enemies/fireball.png");
    spriteShader = LoadShader("shaders/glsl/sprite.vs", "shaders/glsl/sprite.fs");
    spriteViewPosLoc = GetShaderLocation(spriteShader, "viewPos");
    spriteFogDensityLoc = GetShaderLocation(spriteShader, "fogDensity");
    nav = NavGrid(map);
    spawnEnemies();

    // Initialize sounds:
    slashSfx = LoadSound("assets/sfx/slash.wav");
    deathSfx = LoadSound("assets/sfx/death.wav");
    crossbowSfx = LoadSound("assets/sfx/bow.wav");
    gunshotSfx = LoadSound("assets/sfx/musket.mp3");
    blockSfx = LoadSound("assets/sfx/shield_block.wav");
    music = LoadMusicStream("assets/music/forest-ambience.mp3");
    player.death = &deathSfx;
    PlayMusicStream(music);
    DisableCursor(); // Hide the cursor and lock it to the window center so mouse look gets raw deltas
    // Locking moves the cursor to the center, which the next frame would read as a big mouse movement
    // (and pitch the camera into the floor). Polling now makes the center the starting point.
    PollInputEvents();
}


void Game::resetGame()
{
    wd.cWeapon = 0;
    level = 1;
    player.health = 100;
    player.stamina = 100;
    player.gold = 0;
    player.playerPos = {0.0f, 4.0f, 0.0f};
    player.hit = false;
    player.gameOver = false;
    cam3D.position = player.playerPos;
    cam3D.target = Vector3Add(player.playerPos, Vector3{0, 0, 1});
    cam3D.up = {0.0f, 1.0f, 0.0f};
    cam3D.fovy = 60.0f;
    cam3D.projection = CAMERA_PERSPECTIVE;
    weapon[3].ammo = 20;
    weapon[4].ammo = 20;
    spawnEnemies();
}


void Game::update()
{
    UpdateMusicStream(music);
    player.playerPos = cam3D.position;
    player.weapon = weapon[wd.cWeapon];
    UpdateCamera(&cam3D, CAMERA_FIRST_PERSON); // Update camera

    // Wall collision logic:
    for (int i = 0; i < 64; i++) {
        if (walls[i] != nullptr) {
            if (PlayerControl::wallCollision(cam3D.position, *walls[i]) && !noClip) cam3D.position = player.playerPos;
        }
    }

    if (!player.gameOver) {
        handlePlayerControls();
    } else {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) resetGame();
        cam3D.position.x = player.playerPos.x;
        if (cam3D.position.y > 0.5f) cam3D.position.y -= 0.08f;
        cam3D.position.z = player.playerPos.z;
    }

    updateEnemies();

    SetShaderValue(shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);
    // Update the light shader with the cam3D view position
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &cam3D.position.x, SHADER_UNIFORM_VEC3);
}


void Game::draw()
{
    BeginDrawing();
    ClearBackground(DARKGRAY);
    BeginMode3D(cam3D);
    DrawModel(ground->model, {0.0f, 0.0f, 0.0f}, 1.0f, GRAY);

    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
            if (map[x][y] == 1) DrawModel(wall->model, {x * 8.0f - 8.0f, 2.5f, y * 8.0f - 8.0f}, 1.0f, WHITE);

    // Enemies and fireballs are camera-facing sprites. Enemies fade into the fog like the walls,
    // fireballs are drawn without fog so they glow in the dark.
    SetShaderValue(spriteShader, spriteViewPosLoc, &cam3D.position, SHADER_UNIFORM_VEC3);
    SetShaderValue(spriteShader, spriteFogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);
    BeginShaderMode(spriteShader);
    for (const Enemy& enemy : enemies) {
        const float size = enemy.type().spriteSize;
        DrawBillboardRec(cam3D, enemySheets[static_cast<size_t>(enemy.type().kind)], enemy.sourceRect(),
                         Vector3Add(enemy.position, {0.0f, size / 2.0f, 0.0f}), {size, size},
                         enemy.isHitFlashing() ? Color{255, 80, 80, 255} : WHITE);
    }
    EndShaderMode(); // Draws the batch, so the fog can be switched off for the fireballs

    constexpr float noFog = 0.0f;
    SetShaderValue(spriteShader, spriteFogDensityLoc, &noFog, SHADER_UNIFORM_FLOAT);
    BeginShaderMode(spriteShader);
    for (const Fireball& fireball : fireballs)
        DrawBillboardRec(cam3D, fireballSheet, fireball.sourceRect(), fireball.position,
                         {Fireball::SIZE, Fireball::SIZE}, WHITE);
    EndShaderMode();

    EndMode3D();

    // Draw HUD (in UI reference pixels, scaled to the window by ui):
    ui.begin();
    if (player.hit) DrawRectangleV({0, 0}, {ui.width(), ui.height()}, {255, 0, 0, 32});

    if (!player.gameOver) {
        Ui::drawTexture(crosshair, ui.anchor({0.5f, 0.5f}), {0.5f, 0.5f}, 1.0f, {0, 255, 0, 32});

        // The HUD bar spans the full width at the bottom; its contents are placed in hud.png pixels
        const float hudScale = ui.width() / hud.width;
        const Vector2 hudPos = {0, ui.height() - hud.height * hudScale};
        auto hudPoint = [&](float x, float y) { return Vector2{hudPos.x + x * hudScale, hudPos.y + y * hudScale}; };

        // Weapon sits at the bottom center, tucked behind the top of the HUD:
        Ui::drawTexture(weaponTexture[wd.cWeapon], weaponRec,
                        {ui.width() / 2.0f - WEAPON_OFFSET_X, hudPos.y + WEAPON_HUD_OVERLAP + weaponOffsetY},
                        {0.5f, 1.0f});

        Ui::drawTexture(hud, hudPos, {0, 0}, hudScale);
        Ui::drawTextCentered(TextFormat("%03i", player.stamina), hudPoint(HUD_STAMINA_X, HUD_VALUE_Y),
                             HUD_VALUE_FONT_SIZE * hudScale, BLUE);
        Ui::drawTextCentered(TextFormat("%03i", player.health), hudPoint(HUD_HEALTH_X, HUD_VALUE_Y),
                             HUD_VALUE_FONT_SIZE * hudScale, RED);

        if (!player.weapon.melee)
            Ui::drawTextCentered(TextFormat("%03i", player.weapon.ammo), hudPoint(HUD_AMMO_X, HUD_VALUE_Y),
                                 HUD_VALUE_FONT_SIZE * hudScale, YELLOW);

        Ui::drawTextCentered(weapon[wd.cWeapon].name.c_str(), hudPoint(HUD_WEAPON_X, HUD_VALUE_Y),
                             HUD_NAME_FONT_SIZE * hudScale, WHITE);
    }
    ui.end();
    DrawFPS(0, 0); // Draw FPS
    EndDrawing();
}


void Game::runApplication()
{
    update();
    draw();
}


void Game::handlePlayerControls()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !player.attacking) {
        if (player.stamina >= 30 && player.weapon.melee) {
            player.attacking = true;
            player.stamina -= 30;
            wd.weaponFrame++;
            PlaySound(slashSfx);
            playerAttack();
        }
        if (player.weapon.ammo > 0 && !player.weapon.melee) {
            player.attacking = true;
            wd.weaponFrame++;
            weapon[wd.cWeapon].ammo--;
            if (wd.cWeapon == 4) PlaySound(gunshotSfx);
            else PlaySound(crossbowSfx);
            playerAttack();
        }
    }

    if (PlayerControl::mouseWheelDown()) wd.cWeapon = PlayerControl::prevWeapon(wd.cWeapon);

    if (PlayerControl::mouseWheelUp()) wd.cWeapon = PlayerControl::nextWeapon(wd.cWeapon);

    if (player.attacking) {
        if (wd.cWeapon == SWORD_BRONZE || wd.cWeapon == SWORD_IRON) weaponOffsetY = 216.0f;

        weaponRec.x = 400.0f * wd.weaponFrame;
        weaponRec.y = 400.0f;

        if (wd.weaponFrame > 3) {
            wd.weaponFrame = 0;
            weaponRec.y = 0.0f;
            weaponOffsetY = 0.0f;
            player.attacking = false;
        }
        if (wd.cFrame == 5 && wd.weaponFrame < 4) {
            wd.weaponFrame++;
            wd.cFrame = 0;
        }
        wd.cFrame++;
    } else if (wd.cWeapon == SWORD_BRONZE || wd.cWeapon == SWORD_IRON && !player.attacking) weaponOffsetY = 18.0f;
    else weaponOffsetY = 0.0f;

    if (!player.attacking && player.stamina < 100) player.stamina++;

    if (player.hit && !player.gameOver) {
        if (timer > GetMonitorRefreshRate(GetCurrentMonitor()) / 4) {
            timer = 0;
            player.hit = false;
        }
        timer++;
    }

    if (debugMode) {
        if (IsKeyPressed(KEY_F1)) {
            if (!noClip) {
                noClip = true;
                std::cout << "noclip enabled" << std::endl;
            } else {
                noClip = false;
                std::cout << "noclip disabled" << std::endl;
            }
        }

        if (IsKeyPressed(KEY_F2)) {
            if (!debugCoordinates) debugCoordinates = true;
            else debugCoordinates = false;
        }

        if (IsKeyDown(KEY_F3) && fogDensity > 0) fogDensity -= 0.01f;

        if (IsKeyDown(KEY_F4)) fogDensity += 0.01f;

        if (debugCoordinates) std::cout << player.playerPos.x << " " << player.playerPos.y << " " << player.playerPos.z
        << std::endl;
    }
}


void Game::spawnEnemies()
{
    enemies.clear();
    fireballs.clear();
    for (const EnemySpawn& spawn : ENEMY_SPAWNS)
        if (nav.isWalkable(spawn.cell)) enemies.emplace_back(*spawn.type, NavGrid::cellCenter(spawn.cell, 0.0f));
}


void Game::updateEnemies()
{
    // One Dijkstra pass from the player's cell gives every enemy its shortest path
    if (const auto playerCell = NavGrid::worldToCell(player.playerPos)) nav.computeDistances(*playerCell);

    const float dt = GetFrameTime();
    for (Enemy& enemy : enemies) enemy.update(dt, player, nav, walls, fireballs);

    for (Fireball& fireball : fireballs) fireball.update(dt, player, walls);
    std::erase_if(fireballs, [](const Fireball& fireball) { return fireball.isFinished(); });

    // Push overlapping enemies apart so they don't merge while following the same path
    for (size_t i = 0; i < enemies.size(); i++) {
        for (size_t j = i + 1; j < enemies.size(); j++) {
            Enemy& a = enemies[i];
            Enemy& b = enemies[j];
            if (!a.isAlive() || !b.isAlive()) continue;

            const Vector2 apart = {b.position.x - a.position.x, b.position.z - a.position.z};
            const float distance = Vector2Length(apart);
            const float overlap = 2.0f * Enemy::RADIUS - distance;
            if (overlap <= 0.0f || distance < 0.001f) continue;

            const Vector2 push = Vector2Scale(apart, overlap / 2.0f / distance);
            a.move(-push.x, -push.y, walls);
            b.move(push.x, push.y, walls);
        }
    }
}


void Game::playerAttack()
{
    // Hit the nearest living enemy under the crosshair, unless a wall is in the way
    const Ray ray = {cam3D.position, Vector3Normalize(Vector3Subtract(cam3D.target, cam3D.position))};

    float maxDistance = player.weapon.melee ? MELEE_RANGE : std::numeric_limits<float>::max();
    for (const BoundingBox* wall : walls) {
        if (wall == nullptr) continue;
        if (const RayCollision hit = GetRayCollisionBox(ray, *wall); hit.hit) maxDistance = std::min(maxDistance, hit.distance);
    }

    Enemy* target = nullptr;
    for (Enemy& enemy : enemies) {
        if (!enemy.isAlive()) continue;
        if (const RayCollision hit = GetRayCollisionBox(ray, enemy.bounds()); hit.hit && hit.distance < maxDistance) {
            maxDistance = hit.distance;
            target = &enemy;
        }
    }

    if (target != nullptr && target->takeDamage(player.weapon.dmg, cam3D.position) == Enemy::HitResult::Blocked)
        PlaySound(blockSfx);
}
