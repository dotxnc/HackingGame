#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <raylib.h>
#include <raymath.h>
#include <math.h>

#include "os.h"

#define MAX_VIEWMODELS 10

typedef struct Model_t {
    Model* model;
    char name[32];
} Model_t;

typedef struct Viewmodel_t {
    Camera camera;
    Shader shader;
    RenderTexture2D target;
    
    Model_t models[MAX_VIEWMODELS];
    int num_models;
    int index;
    
    float swap_timer;
    bool swapping;
    int swap_to;
    float swap_offset;
} Viewmodel_t;

static Viewmodel_t viewmodel;

void initViewmodel(Shader);
void addViewmodel(const char*, const char*);
void setViewmodel(const char*);
void nextViewmodel();
void prevViewmodel();
void freeViewmodel();
void renderViewmodel();
void drawViewmodel();

#endif
