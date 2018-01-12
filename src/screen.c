#include "screen.h"

static Model screen_desk;
static Model screen_monitor;
static Model screen_viewer;

static Mesh screen_mesh;

void loadScreenModels()
{
    screen_desk = LoadModel("assets/game/Desk.obj");
    screen_monitor = LoadModel("assets/game/Monitor_01.obj");
    screen_mesh = GenMeshPlane(screen_w_gl, screen_h_gl, 256, 256);
    screen_viewer = LoadModelFromMesh(screen_mesh);
}

void freeScreenModels()
{
    UnloadMesh(&screen_mesh);
    UnloadModel(screen_desk);
    UnloadModel(screen_viewer);
    UnloadModel(screen_monitor);
}

void drawScreen(Screen_t* screen)
{
    DrawModel(screen_desk, screen->pos, 1.f, GRAY);
    DrawModel(screen_monitor, Vector3Add(screen->pos, (Vector3){0.3, 2.6, 0}), 1.f, GRAY);
    
    Matrix m = MatrixIdentity();
    m = MatrixMultiply(m, MatrixRotateX(PI/2.f));
    screen_viewer.transform = m;
    
    Vector3 vpos  = {0.f, 0.f, 0.f};
    vpos = Vector3Add(vpos, screen->pos);
    vpos = Vector3Add(vpos, (Vector3){0.3, 2.6, 0});
    vpos = Vector3Add(vpos, (Vector3){0.f, 0.9f, 0.05f});
    screen_viewer.material.maps[MAP_DIFFUSE].texture = screen->texture.texture;
    DrawModel(screen_viewer, vpos, 1.f, WHITE);
}


