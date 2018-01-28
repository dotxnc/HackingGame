#ifndef OS_H
#define OS_H

#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "screen.h"
#include "network.h"

#define MAX_INPUT 45
#define MAX_LINES 22
#define MAX_ARGS 5
#define MAX_USERNAME 16
#define MAX_READOUTS 10

const char* const clist = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()-=_+:;\"'<>?,./";
const char* const boot_lines[MAX_READOUTS] = {
    "booting... OK",
    "loading drive... OK",
    "loading slave... OK",
    "loading ram... OK",
    "loading OS... OK",
    "checking programs... OK",
    "checking master... OK",
    "switching network... OK",
    "connecting NODE... OK",
    "NODE CONNECTED"
};

typedef enum PROGRAM {
    OFFLINE,
    BOOT,
    LOGIN,
    CONSOLE,
    CHAT
} PROGRAM;

typedef struct OS_t {
    char input[MAX_INPUT];
    int input_length;
    char lines[MAX_LINES][MAX_INPUT];
    int line_length;
    
    char chatlog[MAX_LINES][MAX_INPUT];
    int chat_length;
    
    int bootline;
    float bootline_timeout;
    
    // system
    char username[MAX_USERNAME];
    int username_length;
    PROGRAM program;
    bool online;
    bool grabbed;
    bool isskip;
    float ostime;
} OS_t;

OS_t local_os;

void initOS(OS_t*);
void freeOS(OS_t*);
bool commandOS(OS_t*, char*);
void updateOS(OS_t*);
void flashOS(OS_t*, char[MAX_LINES][MAX_INPUT], int);
void pushOS(OS_t*, const char*, bool);
void pushlineOS(OS_t*, const char*);
void pushchatOS(OS_t*, const char*);
void drawOS(OS_t*, Screen_t*);

/// Programs

// Console
void consoleUpdate(OS_t*);
void consoleDraw(OS_t*);
void consoleGrab(OS_t*);

// Chat
void chatUpdate(OS_t*);
void chatDraw(OS_t*);
void chatGrab(OS_t*);

// Offline
void offlineUpdate(OS_t*);
void offlineDraw(OS_t*);
void offlineGrab(OS_t*);

// Boot
void bootUpdate(OS_t*);
void bootDraw(OS_t*);
void bootGrab(OS_t*);

// Login
void loginUpdate(OS_t*);
void loginDraw(OS_t*);
void loginGrab(OS_t*);

#endif
