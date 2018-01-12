
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "screen.h"

typedef struct Cube {
    Vector3 pos;
    int height;
} Cube;

bool shade = false;
bool interacting = false;
bool interacted = false;

const char* const clist = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()-=_+:;\"'<>?,./";
char input[52+1] = {0};
int input_length;
int frames = 0;
float tt = 0;

// width 1.7252
// height 1.36

double GL_WIDTH  = 1.0485;
double GL_HEIGHT = 1.049;

int WIDTH  = 512.0;//
int HEIGHT = 512.0;//

Vector3 monitor_pos = (Vector3){0.3, 2.6, 1.25};

int main(int argc, char** argv)
{
    
    WIDTH *= GL_WIDTH;
    HEIGHT *= GL_HEIGHT;
    
    InitWindow(640, 480, "Hack");
    SetExitKey(KEY_F12);
    SetTargetFPS(60);
    
    Camera camera = {{ 3.0f, 3.75f, 3.0f }, { 0.0f, 1.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 90.0f };
    
    Shader shader = LoadShader("assets/base.glsl", "assets/lighting.glsl");
    shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(shader, "modelMatrix");
    
    Model model = LoadModel("assets/game/Monitor_01.obj");
    model.material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/game/Monitor_01.png");
    model.material.shader = shader;
    Model model2 = LoadModel("assets/game/Desk.obj");
    model2.material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/game/Desk.png");
    model2.material.shader = shader;
    
    Mesh screenmesh = GenMeshPlane(GL_WIDTH, GL_HEIGHT, 256, 256);
    Model screen = LoadModelFromMesh(screenmesh);
    RenderTexture2D screentarget = LoadRenderTexture(WIDTH, HEIGHT);
    SetTextureFilter(screentarget.texture, FILTER_POINT);
    
    loadScreenModels();
    
    Screen_t scr;
    scr.texture = LoadRenderTexture(WIDTH*GL_WIDTH, HEIGHT*GL_HEIGHT);
    
    Cube boxes[16];
    for (int i = 0; i < 16; i++) {
        Vector3 pos = (Vector3){-16+rand()%16, rand()%16, -16+rand()%16};
        int h = pos.y*2;
        boxes[i] = (Cube){pos, h};
    }
    
    RenderTexture2D target = LoadRenderTexture(640, 480);
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    
    while (!WindowShouldClose())
    {
        tt += GetFrameTime()*2; 
        
        if (IsKeyPressed(KEY_F1))
            shade = !shade;
        
        if (!interacting) {
            UpdateCamera(&camera);
        } else {
            if (IsKeyPressed(KEY_ESCAPE)) {
                interacting = false;
            }
        }
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        BeginTextureMode(screentarget);
        DrawRectangle(0, 0, WIDTH, HEIGHT, BLACK);
        if (!interacting) {
            int tw = MeasureText("PRESS <E> TO INTERACT", 40);
            DrawText("PRESS <E> TO INTERACT", WIDTH/2-tw/2, HEIGHT/2-20, 40, LIGHTGRAY);;
            interacted = true;
        } else {
            if (frames > 0) {
                int k = GetKeyPressed();
                bool k_is_allowed = false;
                for (int i = 0; i < strlen(clist); i++) {
                    if (clist[i] == k) {
                        k_is_allowed = true;
                        break;
                    }
                }
                if (k_is_allowed) {
                    if (input_length < 52) {
                        input[input_length] = (char)k;
                        input_length++;
                    }
                } else if (k == KEY_BACKSPACE) {
                    if (input_length > 0) {
                        input[input_length-1] = 0;
                        input[input_length] = '\0';
                        input_length--;
                    }
                } else if (k == 32) {
                    if (input_length < 52) {
                        input[input_length] = ' ';
                        input_length++;
                    }
                } else if (k == KEY_ESCAPE) {
                    for (int k = 0; k < 52; k++) {
                        input[k] = 0;
                        input_length = 0;
                    }
                } else if (k != -1) {
                    // printf("K %d\n", k);
                }
            }
            frames=1;
        }
        if (interacted) {
            int iw = MeasureText(input, 20);
            DrawText(input, 5, 10, 20, GREEN);
            if ((int)tt%2==1) {
                DrawRectangle(5+iw+1, 5, 5, 20, GREEN);
            }
        }
        EndTextureMode();
        
        screen.material.maps[MAP_DIFFUSE].texture = screentarget.texture;
        
        Begin3dMode(camera);
        // DrawModel(model, (Vector3){0.3, 2.6, 0}, 1.0, GRAY);
        DrawModel(model, monitor_pos, 1.0, GRAY);
        DrawModel(model2, (Vector3){0, 0, 0}, 1.f, GRAY);
        
        Matrix m = MatrixIdentity();
        m = MatrixMultiply(m, MatrixRotateX(PI/2.f));
        screen.transform = m;
        DrawModel(screen, (Vector3){monitor_pos.x, monitor_pos.y+0.9, monitor_pos.z-0.05}, 1.f, WHITE);
        
        End3dMode();
        
        // DrawTextureRec(target.texture, (Rectangle){0, 0, 640, -480}, (Vector2){0, 0}, WHITE);
        
        EndDrawing();
        
        if (!interacting) {
            if (IsKeyPressed(KEY_E)) {
                interacting = true;
                frames = 0;
            }
        }
        
    }
    
    CloseWindow();
    
    return 0;
}
