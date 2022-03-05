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
    level = 1;
    player.health = 100;
    player.gold = 0;
    player.gameOver = false;
    player.playerPos = { 0.0f, 0.0f };

    // Define camera:
    camera.position = { player.playerPos.x, 4.0f, player.playerPos.y };
    camera.target = { 0.0f, 1.8f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_FIRST_PERSON); // Set a first person camera mode
}


void Game::resetGame()
{
    level = 1;
    player.health = 100;
    player.gold = 0;
    player.gameOver = false;
    player.playerPos = { 0.0f, 0.0f };

    camera.position = { player.playerPos.x, 4.0f, player.playerPos.y };
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
    player.playerPos = { camera.position.x, camera.position.y };

    std::cout << player.playerPos.x << " " << player.playerPos.y << std::endl;

    UpdateCamera(&camera);                  // Update camera
}


void Game::draw()
{
    BeginDrawing();

    ClearBackground(SKYBLUE);

    if (!player.gameOver)
    {
        BeginMode3D(camera);

        DrawPlane(Vector3{ 0.0f, 0.0f, 0.0f }, Vector2{ 1000.0f, 1000.0f }, BEIGE); // Draw ground

        for (int x = 0; x < 8; x++)
            for (int y = 0; y < 8; y++)
                if (map[x][y] == 1)
                    DrawModel(wall.model, { x * 8.0f - 8.0f, 2.5f, y * 8.0f - 8.0f }, 1.0f, WHITE);

        EndMode3D();
        DrawFPS(0, 0);

        EndDrawing();
    }
}


void Game::runApplication()
{
    update();
}

void Game::deInit()
{
    UnloadTexture(wall.wallTexture);
    UnloadModel(wall.model);
}