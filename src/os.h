#ifndef OS_H
#define OS_H

#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "screen.h"

#define MAX_INPUT 46
#define MAX_LINES 23

const char* const clist = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()-=_+:;\"'<>?,./";

typedef struct OS_t {
    char input[MAX_INPUT];
    int input_length;
    char lines[MAX_LINES][MAX_INPUT];
    int line_length;
    bool grabbed;
    bool isskip;
    float ostime;
} OS_t;

void initOS(OS_t*);
void freeOS(OS_t*);
bool commandOS(OS_t*, char*);
void updateOS(OS_t*);
void flashOS(OS_t*, char[MAX_LINES][MAX_INPUT], int);
void pushlineOS(OS_t*, char[MAX_INPUT]);
void drawOS(OS_t*, Screen_t*);

#endif
