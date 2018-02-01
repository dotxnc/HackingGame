#ifndef RESOURCE_H
#define RESOURCE_H

#include <raylib.h>
#include <string.h>

#define MAX_ID 32
#define MAX_SHADERS 32
#define MAX_MODELS 32
#define MAX_TEXTURES 32

typedef struct ResShader_t {
    Shader shader;
    char ID[MAX_ID];
} ResShader_t;

typedef struct ResModel_t {
    Model model;
    char ID[MAX_ID];
} ResModel_t;

typedef struct ResTexture_t {
    Texture2D texture;
    char ID[MAX_ID];
} ResTexture_t;

typedef struct ResourceManager_t {
    ResShader_t shaders[MAX_SHADERS];
    ResModel_t models[MAX_MODELS];
    ResTexture_t textures[MAX_TEXTURES];
    
    int num_shaders;
    int num_models;
    int num_textures;
} ResourceManager_t;

static ResourceManager_t resource_manager;

bool loadResourceShader(char*, char*, const char*);
bool loadResourceModel(const char*, const char*, const char*);
bool loadResourceTexture(const char*, const char*);
Shader* getResourceShader(const char*);
Model* getResourceModel(const char*);
Texture2D* getResourceTexture(const char*);
void freeResources();

#endif
