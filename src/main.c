
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "screen.h"
#include "os.h"

int WIDTH  = 512.0;//
int HEIGHT = 512.0;//

Vector3 monitor_pos = (Vector3){0.3, 2.6, 1.25};

OS_t os;
Screen_t scr;

void updatePlayerScreen(Screen_t*);

int main(int argc, char** argv)
{
    InitWindow(640, 480, "Hack");
    SetExitKey(KEY_F12);
    SetTargetFPS(60);
    
    
    char flash[MAX_LINES][MAX_INPUT];
    for (int i = 0; i < MAX_LINES/2; i++) {
        strcpy(flash[i], FormatText("This is line %d", i+1));
    }
    flashOS(&os, flash, MAX_LINES/2);
    
    
    WIDTH *= screen_w_gl;
    HEIGHT *= screen_h_gl;
    
    Camera camera = {{ 3.0f, 3.65f, 3.0f }, { 0.0f, 1.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 90.0f };
    Shader shader = LoadShader("assets/base.glsl", "assets/lighting.glsl");
    
    loadScreenModels(shader);
    scr.texture = LoadRenderTexture(WIDTH, HEIGHT);
    
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    
    while (!WindowShouldClose())
    {
        updateOS(&os);
        if (!os.grabbed) {
            UpdateCamera(&camera);
        }
        
        BeginDrawing();
        ClearBackground(BLACK);
        updatePlayerScreen(&scr);
        Begin3dMode(camera);
        drawScreen(&scr);
        End3dMode();
        EndDrawing();
        
    }
    
    CloseWindow();
    
    return 0;
}

void updatePlayerScreen(Screen_t* scr)
{
    BeginTextureMode(scr->texture);
    drawOS(&os, scr);
    EndTextureMode();
}
