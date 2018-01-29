#include "viewmodel.h"

void initViewmodel(Shader shader)
{
    // pos, up, target, fov
    viewmodel.camera = (Camera){{0, 1, 0}, {0, 1, 0}, {0, 0, -1}, 45.f};
    viewmodel.target = LoadRenderTexture(640, 480);
    viewmodel.index = -1;
    viewmodel.shader = shader;
    viewmodel.num_models = 0;
    
    viewmodel.camera.target = QuaternionToEuler(QuaternionFromEuler(0, 0, 0));
    
    SetCameraMode(viewmodel.camera, -1);
}

void addViewmodel(const char* file, const char* name)
{
    if (viewmodel.num_models > MAX_VIEWMODELS-1) return;
    viewmodel.models[viewmodel.num_models].model = LoadModel(file);
    viewmodel.models[viewmodel.num_models].model.material.shader = viewmodel.shader;
    strcpy(viewmodel.models[viewmodel.num_models].name, name);
    viewmodel.num_models++;
}

void setViewmodel(const char* name)
{
    for (int i = 0; i < viewmodel.num_models; i++) {
        if (strcmp(viewmodel.models[i].name, name)==0) {
            viewmodel.index = i;
            break;
        }
    }
}

void freeViewmodel()
{
    for (int i = 0; i < viewmodel.num_models; i++) {
        UnloadModel(viewmodel.models[i].model);
    }
    UnloadRenderTexture(viewmodel.target);
}

static float d = 0.f;

void renderViewmodel()
{
    if (viewmodel.index < 0) return;
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
            viewmodel.models[viewmodel.index].model.transform = m;
            
            DrawModel(viewmodel.models[viewmodel.index].model, (Vector3){0.7, -1, 0.8+sin(d)*0.05}, 1.f, WHITE);
        End3dMode();
    EndTextureMode();
}

void drawViewmodel()
{
    DrawTextureRec(viewmodel.target.texture, (Rectangle){0, 0, 640, -480}, (Vector2){0, 0}, WHITE);
}
