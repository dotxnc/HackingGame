
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>
#include <raymath.h>

#include "network.h"
#include "screen.h"
#include "os.h"

Vector3 monitor_pos = (Vector3){0.3, 2.6, 1.25};

void updatePlayerScreen(Screen_t*);

int main(int argc, char** argv)
{
    initNetwork();
    
    InitWindow(640, 480, "Hack");
    SetExitKey(KEY_F12);
    SetTargetFPS(60);
    
    initOS(NULL);
    
    Shader shader = LoadShader("assets/base.glsl", "assets/lighting.glsl");
    loadScreenModels(shader);
    
    local_screen.pos.x = -20+rand()%40;
    local_screen.pos.z = -20+rand()%40;
    
    int WIDTH = screen_w * screen_w_gl;
    int HEIGHT = screen_h * screen_h_gl;
    
    Camera camera = {{ local_screen.pos.x+3.0f, 3.65f, local_screen.pos.z+3.0f }, { 0.0f, 1.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 90.0f };
    
    Model tower = LoadModel("assets/game/Tower.obj");
    tower.material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/game/Tower.png");
    tower.material.shader = shader;
    
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    
    printf("[INFO] Initialized player at %fx %fz\n", local_screen.pos.x, local_screen.pos.z);
    
    while (!WindowShouldClose())
    {
        updateClientNetwork();
        updateServerNetwork();
        
        updateOS(local_os);
        if (!local_os->grabbed) {
            UpdateCamera(&camera);
        }
        
        BeginDrawing();
            
            ClearBackground((Color){0, 0, 0, 10});
            updatePlayerScreen(&local_screen);
            
            Begin3dMode(camera);
                drawPlayers();
                DrawModel(tower, (Vector3){0, 0, -20}, 1.f, WHITE);
                drawScreen(&local_screen);
            End3dMode();
            
            DrawText(FormatText("%s", local_os->grabbed ? "GRABBED" : "NOT GRABBED"), 10, 10, 20, WHITE);
            drawPlayerList();
        
        EndDrawing();
        
    }
    
    freeNetwork();
    freeOS(NULL);
    freeScreenModels();
    UnloadModel(tower);
    
    CloseWindow();
    
    return 0;
}

void updatePlayerScreen(Screen_t* scr)
{
    BeginTextureMode(scr->texture);
    drawOS(local_os, scr);
    EndTextureMode();
}
