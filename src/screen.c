#include "screen.h"

static Model screen_desk;
static Model screen_monitor;
static Model screen_viewer;

static Mesh screen_mesh;

void loadScreenModels(Shader lighting)
{
    screen_desk = LoadModel("assets/game/Desk_02.obj");
    screen_monitor = LoadModel("assets/game/Monitor_01.obj");
    screen_mesh = GenMeshPlane(screen_w_gl, screen_h_gl, 256, 256);
    screen_viewer = LoadModelFromMesh(screen_mesh);
    
    screen_desk.material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/game/Desk_02.png");
    screen_monitor.material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/game/Monitor_01.png");
    
    screen_desk.material.shader = lighting;
    screen_monitor.material.shader = lighting;
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
    SetTextureFilter(screen->texture.texture, FILTER_POINT);
    
    DrawModel(screen_desk, screen->pos, 1.f, GRAY);
    DrawModel(screen_monitor, Vector3Add(screen->pos, (Vector3){0.3, 2.6, 1.25}), 1.f, GRAY);
    
    Matrix m = MatrixIdentity();
    m = MatrixMultiply(m, MatrixRotateX(PI/2.f));
    screen_viewer.transform = m;
    
    Vector3 vpos  = {0.f, 0.f, 0.f};
    vpos = Vector3Add(vpos, screen->pos);
    vpos = Vector3Add(vpos, (Vector3){0.3, 2.6, 1.25});
    vpos = Vector3Add(vpos, (Vector3){0.f, 0.885f, -0.06f});
    screen_viewer.material.maps[MAP_DIFFUSE].texture = screen->texture.texture;
    DrawModel(screen_viewer, vpos, 1.f, WHITE);
}


