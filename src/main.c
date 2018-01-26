
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>
#include <raymath.h>
#define CAMERA_IMPLEMENTATION
#include "camera.h"

#include "network.h"
#include "screen.h"
#include "os.h"

Vector3 monitor_pos;
float post = 0.f;
PlayerPositionPacket_t ppos;

RenderTexture2D screenspace;
Shader dither;
Shader depth;
Shader posterize;

bool D = false;

void updatePlayerScreen(Screen_t*);

int main(int argc, char** argv)
{
    monitor_pos = (Vector3){0.3, 2.6, 1.25};
    
    initNetwork();
    
    InitWindow(640, 480, "Hack");
    SetExitKey(KEY_F12);
    SetTargetFPS(60);
    
    initOS(NULL);
    
    screenspace = LoadRenderTexture(640, 480);
    
    Shader shader = LoadShader("assets/base.glsl", "assets/lighting.glsl");
    shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(shader, "modelMatrix");
    loadScreenModels(shader);
    
    dither = LoadShader("assets/standard.glsl", "assets/dither.glsl");
    depth = LoadShader("assets/standard.glsl", "assets/depth.glsl");
    posterize = LoadShader("assets/standard.glsl", "assets/posterize.glsl");
    
    local_screen.pos.x = -20+rand()%40;
    local_screen.pos.z = -20+rand()%40;
    
    Camera camera = {{ local_screen.pos.x+3.0f, 3.65f, local_screen.pos.z+3.0f }, { 0.0f, 1.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 90.0f };
    ppos.x = camera.position.x;
    ppos.z = camera.position.z;
    
    Model tower = LoadModel("assets/game/Tower.obj");
    tower.material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/game/Tower.png");
    tower.material.shader = shader;
    
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    
    printf("[INFO] Initialized player at %fx %fz\n", local_screen.pos.x, local_screen.pos.z);
    
    while (!WindowShouldClose())
    {
        if (network->client_running) {
            post += GetFrameTime();
            if (post > 1.f/30.f) {
                post = 0.f;
                ppos.uid = network->client.uid;
                ppos.x = camera.position.x;
                ppos.z = camera.position.z;
                ppos.rot = cameraAngle.x+PI;
                sendDataClient(&ppos, sizeof(PlayerPositionPacket_t), PACKET_POSITION);
            }
        }
        
        updateClientNetwork();
        updateServerNetwork();
        
        updateOS(&local_os);
        if (!local_os.grabbed) {
            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                PLAYER_MOVEMENT_SENSITIVITY = 15.f;
            } else {
                PLAYER_MOVEMENT_SENSITIVITY = 20.f;
            }
            UpdateCamera(&camera);
        }
        
        if (IsKeyPressed(KEY_F1)) {
            D = !D;
        }
        
        
        BeginDrawing();
            // ClearBackground(BLACK);
            updatePlayerScreen(&local_screen);
            
            BeginTextureMode(screenspace);
                ClearBackground(BLACK);
                Begin3dMode(camera);
                    drawPlayers();
                    DrawModel(tower, (Vector3){0, 0, -20}, 1.f, WHITE);
                    drawScreen(&local_screen);
                End3dMode();
            EndTextureMode();
            
            BeginShaderMode(posterize);
                DrawTextureRec(screenspace.texture, (Rectangle){0, 0, 640, -480}, (Vector2){0, 0}, WHITE);
            EndShaderMode();
            
            if (D) {
                BeginShaderMode(depth);
                    Rectangle sourceRec = {0, 0, 640, -480};
                    Rectangle destRec = { 0, 0, sourceRec.width*0.5, abs(sourceRec.height*0.5) };
                    Vector2 origin = { 0, 0 };
                    DrawTexturePro(screenspace.depth, sourceRec, destRec, origin, 0.0f, WHITE);
                EndShaderMode();
            }
        
        EndDrawing();
        
    }
    
    freeNetwork();
    freeOS(NULL);
    freeScreenModels();
    UnloadModel(tower);
    UnloadRenderTexture(screenspace);
    UnloadShader(dither);
    UnloadShader(depth);
    UnloadShader(posterize);
    
    CloseWindow();
    
    return 0;
}

void updatePlayerScreen(Screen_t* scr)
{
    drawOS(&local_os, scr);
}
