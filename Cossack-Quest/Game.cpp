#include "Game.h"

Game::Game()
{
    initGame();
}

Game::~Game()
{
}

void Game::initGame()
{
    wall.wallTexture = LoadTexture("assets/wall.png");
    wall.model = LoadModelFromMesh(GenMeshCube(8.0f, 8.0f, 8.0f));
    wall.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = wall.wallTexture;
    hud = LoadTexture("assets/hud.png");
    //background.bgTexture = LoadTexture("assets/background.png");
    //background.model = LoadModelFromMesh(GenMeshCube(800.0f, 600.0f, 0.0f));
    //background.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = background.bgTexture;
    //skybox.bgTexture = LoadTexture("assets/sky.png");
    //skybox.model = LoadModelFromMesh(GenMeshCube(800.0f, 0.0f, 800.0f));
    //skybox.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skybox.bgTexture;

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
    player.weapon = weapon[cWeapon];
    player.ammo = player.weapon.ammo;
    player.attacking = false;

    weaponPosition = { GetScreenWidth() * 0.325f, GetScreenHeight() * 0.3f };
    weaponRec = { 0, 0, (float)weaponTexture[0].width / 4, (float)weaponTexture[0].height / 2 };

    // Define camera:
    camera.position = player.playerPos;
    camera.target = { 0.0f, 1.8f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_FIRST_PERSON); // Set a first person camera mode

    // Define walls:
    int iterator = 0;

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            if (map[x][y] == 1)
                walls[iterator] = new BoundingBox{ x * 8.0f - 12.0f, 0.0f, y * 8.0f - 12.0f, x * 8.0f - 4.0f, 8.0f, y * 8.0f - 4.0f };
            else
                walls[iterator] = nullptr;
            iterator++;
        }
    }
}


void Game::resetGame()
{
    level = 1;
    player.health = 100;
    player.gold = 0;
    player.gameOver = false;

    camera.position = { 0.0f, 4.0f, 0.0f };
    camera.target = { 0.0f, 1.8f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_FIRST_PERSON); // Set a first person camera mode
}


void Game::update()
{
    // Update
    draw();

    Vector3 oldCamPos = camera.position;

    UpdateCamera(&camera);                  // Update camera
    
    // Wall collision logic:
    if (!noClip)
    {
        for (int i = 0; i < 64; i++)
        {
            if (walls[i] != nullptr)
            {
                if (GameManager::wallCollision(camera.position, *walls[i]))
                    camera.position = oldCamPos;
            }
        }
    }
    
    player.playerPos = camera.position;
    player.weapon = weapon[cWeapon];

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !player.attacking)
    {
        if (player.stamina >= 30)
        {
            player.attacking = true;
            player.stamina -= 30;
            weaponFrame++;
        }
    }

    if (IsKeyPressed(KEY_Q))
        cWeapon = GameManager::prevWeapon();

    if (IsKeyPressed(KEY_E))
        cWeapon = GameManager::nextWeapon();

    if (player.attacking)
    {
        if (cWeapon == 1 || cWeapon == 2)
            weaponPosition = { GetScreenWidth() * 0.325f, GetScreenHeight() * 0.6f };

        weaponRec.x = 400.0f * weaponFrame;
        weaponRec.y = 400.0f;

        if (weaponFrame > 3)
        {
            weaponFrame = 0;
            weaponRec.y = 0.0f;
            weaponPosition = { GetScreenWidth() * 0.325f, GetScreenHeight() * 0.3f };
            player.attacking = false;
        }
        if (cFrame == 5 && weaponFrame < 4)
        {
            weaponFrame++;
            cFrame = 0;
        }
        cFrame++;
    }
    else if (cWeapon == 1 || cWeapon == 2 && !player.attacking)
        weaponPosition = { GetScreenWidth() * 0.325f, GetScreenHeight() * 0.325f };
    else
        weaponPosition = { GetScreenWidth() * 0.325f, GetScreenHeight() * 0.3f };

    if (!player.attacking && player.stamina < 100)
        player.stamina++;

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

    if (debugCoordinates)
        std::cout << player.playerPos.x << " " << player.playerPos.y << " " << player.playerPos.z << std::endl;
}


void Game::draw()
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (!player.gameOver)
    {
        BeginMode3D(camera);

        DrawPlane(Vector3{ 0.0f, 0.0f, 0.0f }, Vector2{ 100.0f, 100.0f }, BEIGE); // Draw ground
        
        DrawPlane(Vector3{ 0.0f, -40.0f, 0.0f }, Vector2{ 400.0f, 400.0f }, DARKGREEN);

        for (int x = 0; x < 8; x++)
            for (int y = 0; y < 8; y++)
                if (map[x][y] == 1)
                    DrawModel(wall.model, { x * 8.0f - 8.0f, 2.5f, y * 8.0f - 8.0f }, 1.0f, WHITE);


        // Draw skybox:
        
        //DrawModel(skybox.model, { 0.0f, 360.0f, 0.0f }, 1.0f, WHITE);
        //DrawModel(background.model, { 0.0f, 60.0f, 400.0f }, 1.0f, WHITE);
        //DrawModel(background.model, { 0.0f, 60.0f, -400.0f }, 1.0f, WHITE);
        //DrawModelEx(background.model, { -400.0f, 60.0f, 0.0f }, { 0, 1, 0 }, 90.0f, { 1, 1, 1 }, WHITE);
        //DrawModelEx(background.model, { 400.0f, 60.0f, 0.0f }, { 0, 1, 0 }, 90.0f, { 1, 1, 1 }, WHITE);


        EndMode3D();

        // Draw HUD:
        DrawTextureRec(weaponTexture[cWeapon], weaponRec, weaponPosition, WHITE);

        DrawTextureEx(hud, { 0, GetScreenHeight() - (GetScreenHeight() * 0.2f) }, 0.0f, 0.000625f * GetScreenWidth(), WHITE);
        DrawText(TextFormat("%03i", player.stamina), GetScreenWidth() * 0.1f, GetScreenHeight() - (GetScreenHeight() * 0.12f), 80, BLUE);
        DrawText(TextFormat("%03i", player.health), GetScreenWidth() * 0.3f, GetScreenHeight() - (GetScreenHeight() * 0.12f), 80, RED);

        if (!player.weapon.melee)
            DrawText(TextFormat("%03i", player.ammo), GetScreenWidth() * 0.625f, GetScreenHeight() - (GetScreenHeight() * 0.12f), 80, YELLOW);

        DrawText(weapon[cWeapon].name.c_str(), GetScreenWidth() * 0.8f, GetScreenHeight() - (GetScreenHeight() * 0.12f), 30, WHITE);

        DrawFPS(0, 0); // Draw FPS

        EndDrawing();
    }
}


void Game::runApplication()
{
    update();
}


void Game::deInit()
{
    for (int i = 0; i < 64; i++)
    {
        if (walls[i] != nullptr)
            delete walls[i];
    }

    for (int i = 0; i < 5; i++)
        UnloadTexture(weaponTexture[i]);

    //UnloadTexture(background.bgTexture);
    //UnloadTexture(skybox.bgTexture);
    //UnloadModel(background.model);
    //UnloadModel(skybox.model);
    
    UnloadTexture(hud);
    UnloadTexture(wall.wallTexture);
    UnloadModel(wall.model);
}