#include "viewmodel.h"

void initViewmodel(const char* model, Shader shader)
{
    // pos, up, target, fov
    viewmodel.camera = (Camera){{0, 1, 0}, {0, 1, 0}, {0, 0, -1}, 45.f};
    viewmodel.model = LoadModel(model);
    viewmodel.target = LoadRenderTexture(640, 480);
    
    viewmodel.camera.target = QuaternionToEuler(QuaternionFromEuler(0, 0, 0));
    viewmodel.model.material.shader = shader;
    
    SetCameraMode(viewmodel.camera, -1);
}

void freeViewmodel()
{
    UnloadModel(viewmodel.model);
    UnloadRenderTexture(viewmodel.target);
}

static float d = 0.f;

void renderViewmodel()
{
    float mod = 1.f;
    if (IsKeyDown(KEY_LEFT_SHIFT) && !local_os.grabbed && IsKeyDown(KEY_W)) {
        mod = 5.f;
    }
    d += GetFrameTime()*mod;
    BeginTextureMode(viewmodel.target);
        ClearBackground((Color){0, 0, 0, 0});
        Begin3dMode(viewmodel.camera);
            
            Matrix m = MatrixIdentity();
            m = MatrixMultiply(m, MatrixRotateX(90*DEG2RAD));
            m = MatrixMultiply(m, MatrixRotateY(-0.8+(sin(d)*-14)*DEG2RAD));
            viewmodel.model.transform = m;
            
            DrawModel(viewmodel.model, (Vector3){0.7, -1, 0.8+sin(d)*0.05}, 1.f, WHITE);
        End3dMode();
    EndTextureMode();
}

void drawViewmodel()
{
    DrawTextureRec(viewmodel.target.texture, (Rectangle){0, 0, 640, -480}, (Vector2){0, 0}, WHITE);
}
