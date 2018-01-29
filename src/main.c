
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <raylib.h>
#include <raymath.h>
#define CAMERA_IMPLEMENTATION
#include "camera.h"

#include "network.h"
#include "screen.h"
#include "os.h"
#include "viewmodel.h"
#include "resource.h"

float post = 0.f;
PlayerPositionPacket_t ppos;
bool D = false;
bool MLOCK = true;

RenderTexture2D screenspace;
Shader depth;
Shader posterize;
Camera camera;

void updatePlayerScreen(Screen_t*);
void drawDebugText();

int main(int argc, char** argv)
{
    // seed random
    srand(time(NULL));
    
    // init window
    InitWindow(640, 480, "Hack");
    SetExitKey(KEY_F12);
    SetTargetFPS(60);
    
    // load resources
    loadResourceShader("assets/shaders/base.vs", "assets/shaders/lighting.fs", "lighting");
    loadResourceShader("assets/shaders/standard.vs", "assets/shaders/depth.fs", "depth");
    loadResourceShader("assets/shaders/standard.vs", "assets/shaders/posterize", "posterize");
    loadResourceModel("assets/models/Tower.obj", "assets/models/Tower.png", "tower");
    
    // init variables
    camera = (Camera){{ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 90.0f };
    Shader shader = LoadShader("assets/shaders/base.vs", "assets/shaders/lighting.fs");
    depth         = LoadShader("assets/shaders/standard.vs", "assets/shaders/depth.fs");
    posterize     = LoadShader("assets/shaders/standard.vs", "assets/shaders/posterize.fs");
    screenspace = LoadRenderTexture(640, 480);
    Model tower = LoadModel("assets/models/Tower.obj");
    
    // set defaults
    Shader* lighting = getResourceShader("lighting");
    lighting->locs[LOC_MATRIX_MODEL] = GetShaderLocation(*lighting, "modelMatrix");
    getResourceModel("tower")->material.shader = *getResourceShader("lighting");
    
    // deprecated
    tower.material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/models/Tower.png");
    tower.material.shader = shader;
    shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(shader, "modelMatrix");
    local_screen.pos.x = -20+rand()%40;
    local_screen.pos.z = -20+rand()%40;
    camera.position = (Vector3){local_screen.pos.x+3.0f, 3.65f, local_screen.pos.z+3.0f};
    ppos.x = camera.position.x;
    ppos.z = camera.position.z;
    
    // init components
    initNetwork();
    initOS(NULL);
    loadScreenModels(*lighting);
    initViewmodel(*lighting);
    addViewmodel("assets/models/Hand.obj", "hand");
    addViewmodel("assets/models/Gun.obj", "gun");
    setViewmodel("hand");
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    
    // test resource loading
    loadResourceShader("assets/shaders/base.vs", "assets/shaders/lighting.fs", "lighting_shader");
    loadResourceModel("assets/models/Gun.obj", NULL, "viewmodel_gun");
    getResourceModel("viewmodel_gun")->material.shader = *getResourceShader("lighting_shader");
    
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
        if (!local_os.grabbed && MLOCK) {
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
        if (IsKeyPressed(KEY_F2)) {
            MLOCK = !MLOCK;
        }
        
        if (MLOCK)
            DisableCursor();
        else
            EnableCursor();
        
        
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
                drawViewmodel();
            EndShaderMode();
            
            if (D) {
                BeginShaderMode(depth);
                    Rectangle sourceRec = {0, 0, 640, -480};
                    Rectangle destRec = { 0, 0, sourceRec.width*0.5, abs((int)(sourceRec.height*0.5)) };
                    Vector2 origin = { 0, 0 };
                    DrawTexturePro(screenspace.depth, sourceRec, destRec, origin, 0.0f, WHITE);
                EndShaderMode();
                drawDebugText();
            }
            
            if (network.client_running) {
                if (network.client.ka_timeout < 30) {
                    DrawText(FormatText("NETWORK TIMEOUT: %2f", network.client.ka_timeout), 0, 0, 30, RED);
                }
            }
            
        
        EndDrawing();
        
    }
    
    freeNetwork();
    freeOS(NULL);
    freeScreenModels();
    freeViewmodel();
    freeResources();
    UnloadModel(tower);
    UnloadRenderTexture(screenspace);
    UnloadShader(depth);
    UnloadShader(posterize);
    
    CloseWindow();
    
    return 0;
}

void updatePlayerScreen(Screen_t* scr)
{
    drawOS(&local_os, scr);
}

void drawDebugText()
{
    static int yoff = (int)(480/2)+10;
    DrawText(FormatText("FPS: %d", GetFPS()), 10, yoff, 20, RAYWHITE);
    DrawText(FormatText("POS: %2f %2f %2f", camera.position.x, camera.position.y, camera.position.z), 10, yoff+15, 20, RAYWHITE);
    DrawText(FormatText("TAR: %2f %2f %2f", camera.target.x, camera.target.y, camera.target.z), 10, yoff+15+15, 20, RAYWHITE);
}
