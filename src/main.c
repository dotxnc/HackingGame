
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include <raylib.h>
#include <raymath.h>

#define CAMERA_IMPLEMENTATION
#include "camera.h"
#include "network.h"
#include "screen.h"
#include "os.h"
#include "viewmodel.h"
#include "resource.h"

inline static float cerp(float to, float from, float time) {
    // return to+(from-to)*time;
    float f = (1-cos(time*PI))*0.5f;
    return to*(1-f)+from*f;
}

inline static float lerp(float to, float from, float time) {
    return (1-time)*to + time*from;
}

float post = 0.f;
PlayerPositionPacket_t ppos;
bool D = false;
bool MLOCK = true;
bool enter_screen_view = false;
bool leave_screen_view = false;
float lerp_view_time = 0;
float lerp_view_rate = 0;
Vector3 old_view_pos = {0};
Vector3 old_view_target = {0};

RenderTexture2D screenspace;
RenderTexture2D depthspace;
Camera camera;
int viewPos;

void updatePlayerScreen(Screen_t*);
void drawDebugText();

int main(int argc, char** argv)
{
    // seed random
    srand(time(NULL));
    
    // init window
    InitWindow(640, 480, "P²");
    SetExitKey(KEY_F12);
    SetTargetFPS(60);
    
    // load resources
    loadResourceShader("assets/shaders/base.vs",      "assets/shaders/lighting.fs",   "lighting");
    loadResourceShader("assets/shaders/standard.vs",  "assets/shaders/depth.fs",      "depth");
    loadResourceShader("assets/shaders/standard.vs",  "assets/shaders/posterize.fs",  "posterize");
    loadResourceShader("assets/shaders/standard.vs",  "assets/shaders/dither.fs",     "dither");
    loadResourceShader("assets/shaders/base.vs",      NULL,                           "distort");
    loadResourceShader("assets/shaders/standard.vs",  "assets/shaders/terminal.fs",   "terminal");
    loadResourceShader("assets/shaders/standard.vs",  "assets/shaders/ssao.fs",       "ssao");
    loadResourceModel("assets/models/Monitor_01.obj", "assets/models/Monitor_01.png", "monitor");
    loadResourceModel("assets/models/Player.obj",     "assets/models/Player.png",     "player");
    loadResourceModel("assets/models/Keyboard.obj",   "assets/models/Keyboard.png",   "keyboard");
    loadResourceModel("assets/models/Mouse.obj",      "assets/models/Mouse.png",      "mouse");
    loadResourceModel("assets/models/Desk_02.obj",    "assets/models/Desk_02.png",    "desk");
    loadResourceModel("assets/models/Tower.obj",      "assets/models/Tower.png",      "tower");
    loadResourceModel("assets/models/Gun.obj",        "assets/models/Gun.png",        "viewmodel_gun");
    loadResourceModel("assets/models/Hand.obj",       "assets/models/Hand.png",       "viewmodel_hand");
    loadResourceTexture("assets/textures/noise.png", "noise");
    
    // init variables
    camera = (Camera){{ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 90.0f };
    screenspace = LoadRenderTexture(640, 480);
    depthspace = LoadRenderTexture(640, 480);
    
    // set defaults
    Shader* lighting = getResourceShader("lighting");
    lighting->locs[LOC_MATRIX_MODEL] = GetShaderLocation(*lighting, "modelMatrix");
    Shader* ssao = getResourceShader("ssao");
    
    viewPos = GetShaderLocation(*lighting, "viewPos");
    getResourceModel("tower")->material.shader = *lighting;
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
    addViewmodel("viewmodel_hand", "hand");
    addViewmodel("viewmodel_gun", "gun");
    setViewmodel("hand");
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
        
        
        if (!local_os.grabbed && IsKeyPressed(KEY_E)) {
            leave_screen_view = false;
            enter_screen_view = true;
            lerp_view_time = 0;
            lerp_view_rate = 0;
            old_view_pos = camera.position;
            old_view_target = camera.target;
        }
        // Lerp to local os
        if (local_os.grabbed && IsKeyPressed(KEY_ESCAPE)) {
            enter_screen_view = false;
            leave_screen_view = true;
            lerp_view_time = 0;
            lerp_view_rate = 0;
        }
        updateOS(&local_os);
        if (!local_os.grabbed && MLOCK) {
            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                PLAYER_MOVEMENT_SENSITIVITY = 15.f;
            } else {
                PLAYER_MOVEMENT_SENSITIVITY = 20.f;
            }
            
            if (!enter_screen_view && !leave_screen_view)
                UpdateCamera(&camera);
        }
        if (enter_screen_view) {
            lerp_view_rate = 1.0f/2.f;
            if (lerp_view_time < 1.f) {
                lerp_view_time += GetFrameTime() * lerp_view_rate;
                Vector3 topos = {local_screen.pos.x+screen_offset.x, local_screen.pos.y+screen_offset.y+0.88, local_screen.pos.z+screen_offset.z+0.5};
                camera.position.x = cerp(old_view_pos.x, topos.x, lerp_view_time);
                camera.position.y = cerp(old_view_pos.y, topos.y, lerp_view_time);
                camera.position.z = cerp(old_view_pos.z, topos.z, lerp_view_time);
                
                camera.target = (Vector3){topos.x, topos.y, topos.z-1};
            }
        }
        
        if (leave_screen_view) {
            lerp_view_rate = 1.0f/2.f;
            if (lerp_view_time < 1.f) {
                lerp_view_time += GetFrameTime() * lerp_view_rate;
                camera.position.x = lerp(local_screen.pos.x+screen_offset.x, old_view_pos.x, lerp_view_time);
                camera.position.y = lerp(local_screen.pos.y+screen_offset.y+0.88, old_view_pos.y, lerp_view_time);
                camera.position.z = lerp(local_screen.pos.z+screen_offset.z+0.5, old_view_pos.z, lerp_view_time);
                
                camera.target = old_view_target;
            } else {
                leave_screen_view = false;
            }
        }
        
        SetShaderValue(*lighting, viewPos, Vector3ToFloat(camera.position), 3);
        
        if (IsKeyPressed(KEY_F1)) {
            D = !D;
        }
        if (IsKeyPressed(KEY_F2)) {
            MLOCK = !MLOCK;
        }
        if (GetMouseWheelMove()>0) {
            nextViewmodel();
        }
        else if (GetMouseWheelMove()<0) {
            prevViewmodel();
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
                    DrawModel(*getResourceModel("tower"), (Vector3){0, 0, -20}, 1.f, WHITE);
                    drawScreen(&local_screen);
                End3dMode();
            EndTextureMode();
            
            BeginShaderMode(*getResourceShader("posterize"));
                DrawTextureRec(screenspace.texture, (Rectangle){0, 0, 640, -480}, (Vector2){0, 0}, WHITE);
                drawViewmodel();
            EndShaderMode();
            
            if (D) {
                BeginTextureMode(depthspace);
                    BeginShaderMode(*getResourceShader("depth"));
                        Rectangle sourceRec = {0, 0, 640, -480};
                        Rectangle destRec = { 0, 0, sourceRec.width, abs((sourceRec.height)) };
                        Vector2 origin = { 0, 0 };
                        DrawTexturePro(screenspace.depth, sourceRec, destRec, origin, 0.0f, WHITE);
                    EndShaderMode();
                EndTextureMode();
                
                destRec = (Rectangle){ 0, 0, sourceRec.width*0.5, abs((int)(sourceRec.height*0.5)) };
                DrawTexturePro(screenspace.texture, sourceRec, destRec, origin, 0.0f, WHITE);
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
    UnloadRenderTexture(screenspace);
    
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
    DrawText(FormatText("MAX_INPUT TEST: %d %d", local_os.input_length, MAX_INPUT), 10, yoff+15+15+15, 20, RAYWHITE);
}
