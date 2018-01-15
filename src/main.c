
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>
#include <raymath.h>

#include "network.h"
#include "screen.h"
#include "os.h"

int WIDTH  = 512.0;//
int HEIGHT = 512.0;//

Vector3 monitor_pos = (Vector3){0.3, 2.6, 1.25};

Screen_t scr;

void updatePlayerScreen(Screen_t*);

int main(int argc, char** argv)
{
    initNetwork();
    
    InitWindow(640, 480, "Hack");
    SetExitKey(KEY_F12);
    SetTargetFPS(60);
    
    initOS(NULL);
    
    // char flash[MAX_LINES][MAX_INPUT];
    // for (int i = 0; i < MAX_LINES/2; i++) {
    //     strcpy(flash[i], FormatText("This is line %d", i+1));
    // }
    // flashOS(local_os, flash, MAX_LINES/2);
    
    WIDTH *= screen_w_gl;
    HEIGHT *= screen_h_gl;
    
    Camera camera = {{ 3.0f, 3.65f, 3.0f }, { 0.0f, 1.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 90.0f };
    Shader shader = LoadShader("assets/base.glsl", "assets/lighting.glsl");
    
    loadScreenModels(shader);
    scr.texture = LoadRenderTexture(WIDTH, HEIGHT);
    
    Model tower = LoadModel("assets/game/Tower.obj");
    tower.material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/game/Tower.png");
    tower.material.shader = shader;
    
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    
    while (!WindowShouldClose())
    {
        updateClientNetwork();
        // updateServerNetwork();
        
        updateOS(local_os);
        if (!local_os->grabbed) {
            UpdateCamera(&camera);
        }
        
        BeginDrawing();
        
        ClearBackground(BLACK);
        updatePlayerScreen(&scr);
        
        Begin3dMode(camera);
        DrawModel(tower, (Vector3){0, 0, -20}, 1.f, WHITE);
        drawScreen(&scr);
        End3dMode();
        
        EndDrawing();
        
    }
    
    freeNetwork();
    freeScreenModels();
    UnloadModel(tower);
    freeOS(NULL);
    
    CloseWindow();
    
    return 0;
}

void updatePlayerScreen(Screen_t* scr)
{
    BeginTextureMode(scr->texture);
    drawOS(local_os, scr);
    EndTextureMode();
}
