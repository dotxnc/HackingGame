#ifndef NETWORK_H
#define NETWORK_H

#define WITHOUT_SDL
#define CloseWindow CloseWindow_orig
#define Rectangle Rectangle_orig
#define ShowCursor ShowCursor_orig
#define LoadImage LoadImage_orig
#define DrawText DrawText_orig
#define DrawTextEx DrawTextEx_orig
#define PlaySound PlaySound_orig
#include <winsock2.h>
#undef CloseWindow
#undef Rectangle
#undef ShowCursor
#undef LoadImage
#undef DrawText
#undef DrawTextEx
#undef PlaySound

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "os.h"

typedef struct ServerData_t {
    SOCKET socket;
    struct sockaddr_in addr, si_other;
    int slen, recv_len;
    char buffer[512];
    WSADATA wsa;
} ServerData_t;

typedef struct ClientData_t {
    SOCKET socket;
    struct sockaddr_in si_other;
    int slen, recv_len;
    char buffer[512];
    WSADATA wsa;
} ClientData_t;

typedef struct Network_t {
    bool server_running;
    bool client_running;
    ServerData_t server;
    ClientData_t client;
} Network_t;

static Network_t* network;

bool initNetwork();
void freeNetwork();
bool startServerNetwork(int);
bool startClientNetwork(const char*, int);
void updateServerNetwork();
void updateClientNetwork();

#endif
