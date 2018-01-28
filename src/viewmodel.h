#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <raylib.h>
#include <raymath.h>
#include <math.h>

#include "os.h"

typedef struct Viewmodel_t {
    Camera camera;
    Model model;
    RenderTexture2D target;
} Viewmodel_t;

static Viewmodel_t viewmodel;

void initViewmodel(const char*, Shader);
void freeViewmodel();
void renderViewmodel();
void drawViewmodel();

#endif
