#ifndef SCREEN_H
#define SCREEN_H

#include <raylib.h>
#include <raymath.h>

typedef struct Screen_t {
    bool in_use;
    Vector3 pos;
    RenderTexture2D texture;
} Screen_t;

const double screen_w_gl = 1.0485;
const double screen_h_gl = 1.049;

void loadScreenModels(Shader);
void freeScreenModels();
Screen_t loadScreen(int, int);
void drawScreen(Screen_t*);

#endif
