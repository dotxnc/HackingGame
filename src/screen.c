#include "screen.h"

static Model* screen_desk;
static Model* screen_monitor;
static Model* screen_keyboard;
static Model* screen_mouse;
static Model* screen_player;

static Model screen_viewer;
static Mesh screen_mesh;
static Vector3 screen_offset;

void loadScreenModels(Shader lighting)
{
    screen_desk = getResourceModel("desk");
    screen_monitor = getResourceModel("monitor");
    screen_keyboard = getResourceModel("keyboard");
    screen_mouse = getResourceModel("mouse");
    screen_player = getResourceModel("player");;
    
    screen_mesh = GenMeshPlane(screen_w_gl, screen_h_gl, 5, 5);
    screen_viewer = LoadModelFromMesh(screen_mesh);
    
    screen_desk->material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/models/Desk_02.png");
    screen_monitor->material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/models/Monitor_01.png");
    screen_keyboard->material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/models/Keyboard.png");
    screen_mouse->material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/models/Mouse.png");
    screen_player->material.maps[MAP_DIFFUSE].texture = LoadTexture("assets/models/Player.png");
    
    screen_desk->material.shader = lighting;
    screen_monitor->material.shader = lighting;
    screen_keyboard->material.shader = lighting;
    screen_mouse->material.shader = lighting;
    screen_player->material.shader = lighting;
    
    screen_viewer.material.shader = lighting;
    
    local_screen.in_use = false;
    local_screen.texture = LoadRenderTexture(screen_w, screen_h);
    
    screen_offset.x = -0.4f;
    screen_offset.y = 2.6f;
    screen_offset.z = 0.5f;
}

void freeScreenModels()
{
    UnloadModel(screen_viewer);
    UnloadRenderTexture(local_screen.texture);
}

void drawScreen(Screen_t* screen)
{
    SetTextureFilter(screen->texture.texture, FILTER_POINT);
    
    DrawModel(*screen_desk, screen->pos, 1.f, GRAY);
    DrawModel(*screen_monitor,  Vector3Add(screen->pos, screen_offset), 1.f, GRAY);
    DrawModel(*screen_keyboard, Vector3Add(screen->pos, screen_offset), 1.f, GRAY);
    DrawModel(*screen_mouse,    Vector3Add(screen->pos, screen_offset), 1.f, GRAY);
    
    Matrix m = MatrixIdentity();
    m = MatrixMultiply(m, MatrixRotateX(PI/2.f));
    m = MatrixMultiply(m, MatrixRotateY(PI));
    screen_viewer.transform = m;
    
    Vector3 vpos  = {0.f, 0.f, 0.f};
    vpos = Vector3Add(vpos, screen->pos);
    vpos = Vector3Add(vpos, screen_offset);
    vpos = Vector3Add(vpos, (Vector3){0.f, 0.885f, -0.07f});
    // vpos = Vector3Add(vpos, (Vector3){0.f, 0.885f, 0.1f});
    screen_viewer.material.maps[MAP_DIFFUSE].texture = screen->texture.texture;
    DrawModel(screen_viewer, vpos, 1.f, WHITE);
}

void drawPlayer(Screen_t* screen, float rot)
{
    Matrix m = MatrixIdentity();
    m = MatrixMultiply(m, MatrixRotateY(rot));
    screen_player->transform = m;
    DrawModel(*screen_player, screen->realpos, 0.85f, GRAY);
}


