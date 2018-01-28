
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
#include "viewmodel.h"

Vector3 monitor_pos;

float post = 0.f;
PlayerPositionPacket_t ppos;
bool D = false;

RenderTexture2D screenspace;
Shader dither;
Shader depth;
Shader posterize;


void updatePlayerScreen(Screen_t*);

int main(int argc, char** argv)
{
    // init window
    InitWindow(640, 480, "Hack");
    SetExitKey(KEY_F12);
    SetTargetFPS(60);
    
    // init variables
    Camera camera = {{ local_screen.pos.x+3.0f, 3.65f, local_screen.pos.z+3.0f }, { 0.0f, 1.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 90.0f };
    Shader shader = LoadShader("assets/shaders/base.vs", "assets/shaders/lighting.fs");
    dither        = LoadShader("assets/shaders/standard.vs", "assets/shaders/dither.fs");
    depth         = LoadShader("assets/shaders/standard.vs", "assets/shaders/depth.fs");
    posterize     = LoadShader("assets/shaders/standard.vs", "assets/shaders/posterize.fs");
    screenspace = LoadRenderTexture(640, 480);
    Model tower = LoadModel("assets/models/Tower.obj");
    
    // set defaults
    monitor_pos = (Vector3){0.3, 2.6, 1.25};
    tower.material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/models/Tower.png");
    tower.material.shader = shader;
    shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(shader, "modelMatrix");
    local_screen.pos.x = -20+rand()%40;
    local_screen.pos.z = -20+rand()%40;
    ppos.x = camera.position.x;
    ppos.z = camera.position.z;
    
    // init components
    initNetwork();
    initOS(NULL);
    loadScreenModels(shader);
    initViewmodel("assets/models/Hand.obj", shader);
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    
    printf("[INFO] Initialized player at %fx %fz\n", local_screen.pos.x, local_screen.pos.z);
    
    while (!WindowShouldClose())
    {
        if (network.client_running) {
            post += GetFrameTime();
            if (post > 1.f/30.f) {
                post = 0.f;
                ppos.uid = network.client.uid;
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
            ClearBackground(BLACK);
            updatePlayerScreen(&local_screen);
            
            renderViewmodel();
            
            BeginTextureMode(screenspace);
                Begin3dMode(camera);
                    drawPlayers();
                    DrawModel(tower, (Vector3){0, 0, -20}, 1.f, WHITE);
                    drawScreen(&local_screen);
                End3dMode();
            EndTextureMode();
            
            BeginShaderMode(posterize);
                DrawTextureRec(screenspace.texture, (Rectangle){0, 0, 640, -480}, (Vector2){0, 0}, WHITE);
            EndShaderMode();
            drawViewmodel();
            
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
    freeViewmodel();
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
