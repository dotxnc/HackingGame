#include "viewmodel.h"

inline static float lerp(float to, float from, float time) {
    // return to+(from-to)*time;
    float f = (1-cos(time*PI))*0.5f;
    return to*(1-f)+from*f;
}

void initViewmodel(Shader shader)
{
    // pos, up, target, fov
    viewmodel.camera = (Camera){{0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 45.f};
    viewmodel.target = LoadRenderTexture(640, 480);
    viewmodel.index = -1;
    viewmodel.shader = shader;
    viewmodel.num_models = 0;
    viewmodel.swap_timer = 0;
    viewmodel.swap_offset = 0;
    viewmodel.swap_to = -1;
    viewmodel.swapping = false;
    
    // viewmodel.camera.target = QuaternionToEuler(QuaternionFromEuler(0, 0, 0));
    
    // SetCameraMode(viewmodel.camera, -1);
}

void addViewmodel(const char* resource, const char* name)
{
    if (viewmodel.num_models > MAX_VIEWMODELS-1) return;
    viewmodel.models[viewmodel.num_models].model = getResourceModel(resource);
    viewmodel.models[viewmodel.num_models].model->material.shader = viewmodel.shader;
    strcpy(viewmodel.models[viewmodel.num_models].name, name);
    viewmodel.num_models++;
}

void setViewmodel(const char* name)
{
    for (int i = 0; i < viewmodel.num_models; i++) {
        if (strcmp(viewmodel.models[i].name, name)==0) {
            viewmodel.swapping = true;
            viewmodel.swap_to = i;
            viewmodel.swap_timer = 0;
            break;
        }
    }
}

void nextViewmodel()
{
    viewmodel.swap_to = (viewmodel.index+1)%viewmodel.num_models;
    viewmodel.swapping = true;
}

void prevViewmodel()
{
    if (viewmodel.index == 0)
        viewmodel.swap_to = viewmodel.num_models-1;
    else
        viewmodel.swap_to--;
    viewmodel.swapping = true;
}

void freeViewmodel()
{
    UnloadRenderTexture(viewmodel.target);
}

static float d = 0.f;

void renderViewmodel()
{
    if (viewmodel.swapping) {
        if (viewmodel.swap_offset < 0.05) {
            viewmodel.swap_offset += GetFrameTime();//lerp(3, viewmodel.swap_offset, 0.98);
        } else {
            viewmodel.swapping = false;
            viewmodel.index = viewmodel.swap_to;
        }
    } else {
        if (viewmodel.swap_offset > 0)
            viewmodel.swap_offset -= GetFrameTime();//lerp(0, viewmodel.swap_offset, 0.98);
    }
    
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
            // m = MatrixMultiply(m, MatrixRotateX(90*DEG2RAD));
            m = MatrixMultiply(m, MatrixRotateZ(-0.2+(sin(d)*-14)*DEG2RAD));
            viewmodel.models[viewmodel.index].model->transform = m;
            
            DrawModel(*viewmodel.models[viewmodel.index].model, (Vector3){0.7, -0.9, -2.2}, 1.f, WHITE);
        End3dMode();
    EndTextureMode();
}

void drawViewmodel()
{
    DrawTextureRec(viewmodel.target.texture, (Rectangle){0, 0, 640, -480}, (Vector2){0, 0}, WHITE);
}
