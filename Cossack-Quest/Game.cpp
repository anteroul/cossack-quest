#include "Game.hpp"

#define GLSL_VERSION  330

#define RLIGHTS_IMPLEMENTATION
#include "shaders/rlights.h"

Game::Game(int wWidth, int wHeight, const char* wTitle, bool fullscreenEnabled, bool consoleEnabled)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1280, 720, "The Last Cossack");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
    if (fullscreenEnabled) ToggleFullscreen();
    debugMode = consoleEnabled;
    windowSize = {static_cast<float>(wWidth), static_cast<float>(wHeight)};
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
    CloseAudioDevice();

    for (auto& i : weaponTexture)
        UnloadTexture(i);

    UnloadTexture(hud);
    UnloadTexture(crosshair);
    UnloadShader(shader);

    for (auto& i : walls)
    {
        delete i;
    }

    delete enemy;
    delete wall;
    delete ground;

    CloseWindow();
}

void Game::initGame()
{
    // Initialize textures and models:
    ground = new GameObject({0.0f, 0.0f, 0.0f}, LoadTexture("assets/dirt.png"), LoadModelFromMesh(GenMeshPlane(8.0f * 16, 8.0f * 16, 8, 8)));
    ground->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = ground->texture;
    wall = new GameObject({0.0f, 0.0f, 0.0f}, LoadTexture("assets/wall.png"), LoadModelFromMesh(GenMeshCube(8.0f, 8.0f, 8.0f)));
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
    player.playerPos = { 0.0f, 4.0f, 0.0f };
    player.weapon = weapon[wd.cWeapon];
    player.attacking = false;

    weaponPosition = { GetScreenWidth() * 0.325f, GetScreenHeight() * 0.3f };
    weaponRec = { 0, 0, (float)weaponTexture[0].width / 4, (float)weaponTexture[0].height / 2 };

    // Define walls:
    int iterator = 0;

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            if (map[x][y] == 1)
                walls[iterator] = new BoundingBox{ x * 8.0f - 12.0f, 0.0f, y * 8.0f - 12.0f, x * 8.0f - 4.0f, 8.0f, y * 8.0f - 4.0f };
            else
                walls[iterator] = nullptr;
            iterator++;
        }
    }

    // Initialize our enemy:
    enemy = new Enemy({}, LoadTexture("assets/default.png"), LoadModel("assets/cultist_mage.glb"), walls);
    enemy->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = enemy->texture;
    placeEnemyToGrid(enemy, 6, 6);

    // Initialize shaders:
    shader = LoadShader("shaders/glsl/base_lighting.vs", "shaders/glsl/fog.fs");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    ambientLoc = GetShaderLocation(shader, "ambient");
    fogDensityLoc = GetShaderLocation(shader, "fogDensity");
    SetShaderValue(shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);

    // Define cam3D:
    cam3D.position = player.playerPos;
    cam3D.target = Vector3Add(player.playerPos, Vector3{ 0,0,1 });
    cam3D.up = { 0.0f, 1.0f, 0.0f };
    cam3D.fovy = 60.0f;
    cam3D.projection = CAMERA_PERSPECTIVE;

    // Using just 1 point lights
    CreateLight(LIGHT_DIRECTIONAL, { -100, 10, -100 }, Vector3Zero(), DARKGRAY, shader);
    CreateLight(LIGHT_DIRECTIONAL, { -100, 10, 100 }, Vector3Zero(), DARKGRAY, shader);
    CreateLight(LIGHT_DIRECTIONAL, { 100, 10, 100 }, Vector3Zero(), DARKGRAY, shader);
    CreateLight(LIGHT_DIRECTIONAL, { 100, 10, -100 }, Vector3Zero(), DARKGRAY, shader);

    // Apply shaders for 3D game objects
    wall->model.materials[0].shader = shader;
    enemy->model.materials[0].shader = shader;

    // Initialize sounds:
    slashSfx = LoadSound("assets/sfx/slash.wav");
    deathSfx = LoadSound("assets/sfx/death.wav");
    crossbowSfx = LoadSound("assets/sfx/bow.wav");
    gunshotSfx = LoadSound("assets/sfx/musket.mp3");
    music = LoadMusicStream("assets/music/forest-ambience.mp3");
    player.death = &deathSfx;
    PlayMusicStream(music);
    HideCursor();
}


void Game::resetGame()
{
    wd.cWeapon = 0;
    level = 1;
    player.health = 100;
    player.stamina = 100;
    player.gold = 0;
    player.playerPos = { 0.0f, 4.0f, 0.0f };
    player.hit = false;
    player.gameOver = false;
    cam3D.position = player.playerPos;
    cam3D.target = Vector3Add(player.playerPos, Vector3{ 0,0,1 });
    cam3D.up = { 0.0f, 1.0f, 0.0f };
    cam3D.fovy = 60.0f;
    cam3D.projection = CAMERA_PERSPECTIVE;
    weapon[3].ammo = 20;
    weapon[4].ammo = 20;
    placeEnemyToGrid(enemy, 6, 6);
}


void Game::update()
{
    UpdateMusicStream(music);
    player.playerPos = cam3D.position;
    player.weapon = weapon[wd.cWeapon];
    UpdateCamera(&cam3D, CAMERA_FIRST_PERSON);                  // Update camera
    PlayerControl::placeCursorMiddle(windowSize, GetMousePosition());
    
    // Wall collision logic:
    for (int i = 0; i < 64; i++)
    {
        if (walls[i] != nullptr)
        {
            if (PlayerControl::wallCollision(cam3D.position, *walls[i]) && !noClip)
                cam3D.position = player.playerPos;
        }
    }

    if (!player.gameOver)
    {
        handlePlayerControls();
    } else {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) resetGame();
        cam3D.position.x = player.playerPos.x;
        if (cam3D.position.y > 0.5f) cam3D.position.y -= 0.08f;
        cam3D.position.z = player.playerPos.z;
    }
    
    enemy->update(&player);

    SetShaderValue(shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);

    // Update the light shader with the cam3D view position
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &cam3D.position.x, SHADER_UNIFORM_VEC3);

}


void Game::draw()
{
    BeginDrawing();
    ClearBackground(DARKGRAY);
    BeginMode3D(cam3D);
    DrawModel(ground->model, { 0.0f, 0.0f, 0.0f }, 1.0f, GRAY);

    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
            if (map[x][y] == 1)
                DrawModel(wall->model, { x * 8.0f - 8.0f, 2.5f, y * 8.0f - 8.0f }, 1.0f, WHITE);

    DrawModel(enemy->model, enemy->position, 0.02f, BLACK);
    //DrawBoundingBox(enemy->getBounds(), GREEN);
    EndMode3D();

    // Draw HUD:
    if (player.hit) DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), { 255, 0, 0, 32 });

    if (!player.gameOver)
    {
        DrawTexture(crosshair, GetScreenWidth() / 2 - (crosshair.width / 2), GetScreenHeight() / 2 - (crosshair.height / 2), { 0, 255, 0, 32 });
        DrawTextureRec(weaponTexture[wd.cWeapon], weaponRec, weaponPosition, WHITE);
        DrawTextureEx(hud, { 0, GetScreenHeight() - (GetScreenHeight() * 0.2f) }, 0.0f, 0.000625f * GetScreenWidth(), WHITE);
        DrawText(TextFormat("%03i", player.stamina), GetScreenWidth() * 0.1f, GetScreenHeight() - (GetScreenHeight() * 0.12f), 80, BLUE);
        DrawText(TextFormat("%03i", player.health), GetScreenWidth() * 0.3f, GetScreenHeight() - (GetScreenHeight() * 0.12f), 80, RED);

        if (!player.weapon.melee)
            DrawText(TextFormat("%03i", player.weapon.ammo), GetScreenWidth() * 0.625f, GetScreenHeight() - (GetScreenHeight() * 0.12f), 80, YELLOW);

        DrawText(weapon[wd.cWeapon].name.c_str(), GetScreenWidth() * 0.8f, GetScreenHeight() - (GetScreenHeight() * 0.12f), 30, WHITE);
    }
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
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !player.attacking)
    {
        if (player.stamina >= 30 && player.weapon.melee)
        {
            player.attacking = true;
            player.stamina -= 30;
            wd.weaponFrame++;
            PlaySound(slashSfx);
        }
        if (player.weapon.ammo > 0 && !player.weapon.melee)
        {
            player.attacking = true;
            wd.weaponFrame++;
            weapon[wd.cWeapon].ammo--;
            if (wd.cWeapon == 4) PlaySound(gunshotSfx);
            else PlaySound(crossbowSfx);
        }
    }

    if (PlayerControl::mouseWheelDown())
        wd.cWeapon = PlayerControl::prevWeapon(wd.cWeapon);

    if (PlayerControl::mouseWheelUp())
        wd.cWeapon = PlayerControl::nextWeapon(wd.cWeapon);

    if (player.attacking)
    {
        if (wd.cWeapon == SWORD_BRONZE || wd.cWeapon == SWORD_IRON)
            weaponPosition = { GetScreenWidth() * 0.325f, GetScreenHeight() * 0.6f };

        weaponRec.x = 400.0f * wd.weaponFrame;
        weaponRec.y = 400.0f;

        if (wd.weaponFrame > 3)
        {
            wd.weaponFrame = 0;
            weaponRec.y = 0.0f;
            weaponPosition = { GetScreenWidth() * 0.325f, GetScreenHeight() * 0.3f };
            player.attacking = false;
        }
        if (wd.cFrame == 5 && wd.weaponFrame < 4)
        {
            wd.weaponFrame++;
            wd.cFrame = 0;
        }
        wd.cFrame++;
    }
    else if (wd.cWeapon == SWORD_BRONZE || wd.cWeapon == SWORD_IRON && !player.attacking)
        weaponPosition = { GetScreenWidth() * 0.325f, GetScreenHeight() * 0.325f };
    else
        weaponPosition = { GetScreenWidth() * 0.325f, GetScreenHeight() * 0.3f };

    if (!player.attacking && player.stamina < 100)
        player.stamina++;

    if (player.hit && !player.gameOver)
    {
        if (timer > GetMonitorRefreshRate(GetCurrentMonitor()) / 4)
        {
            timer = 0;
            player.hit = false;
        }
        timer++;
    }

    if (debugMode)
    {
        if (IsKeyPressed(KEY_F1))
        {
            if (!noClip)
            {
                noClip = true;
                std::cout << "noclip enabled" << std::endl;
            }
            else
            {
                noClip = false;
                std::cout << "noclip disabled" << std::endl;
            }
        }

        if (IsKeyPressed(KEY_F2))
        {
            if (!debugCoordinates)
                debugCoordinates = true;
            else
                debugCoordinates = false;
        }

        if (IsKeyDown(KEY_F3) && fogDensity > 0)
            fogDensity -= 0.01f;

        if (IsKeyDown(KEY_F4))
            fogDensity += 0.01f;

        if (debugCoordinates)
            std::cout << player.playerPos.x << " " << player.playerPos.y << " " << player.playerPos.z << std::endl;
    }
}


void Game::placeEnemyToGrid(Enemy* enemy, int x, int y)
{
    x -= 1;
    y -= 1;
    enemy->position = { x * 8.0f, 3.8f, y * 8.0f };
}