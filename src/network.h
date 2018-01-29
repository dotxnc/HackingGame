#ifndef NETWORK_H
#define NETWORK_H

#ifdef _WIN32
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
    #define HACK_GETERROR() WSAGetLastError()
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <sys/unistd.h>
    #include <arpa/inet.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define ioctlsocket ioctl
    #define HACK_GETERROR() "Error not specified on *nix"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "os.h" 
#include "packets.h"
#include "screen.h"

#define MAX_CLIENTS 8
#define DEFAULT_PORT 27015
#define DEFAULT_IP "127.0.0.1"

typedef struct ClientInfo_t {
    int uid;
    int slen;
    struct sockaddr_in si_other;
    float x;
    float z;
    float rx;
    float rz;
    float rot;
} ClientInfo_t;

typedef struct PlayerInfo_t {
    int uid;
    float x;
    float z;
    float rx;
    float rz;
    float rot;
    Screen_t screen;
} PlayerInfo_t;

typedef struct ServerData_t {
    #ifdef _WIN32
        WSADATA wsa;
        SOCKET socket;
    #else
        int socket;
    #endif
    struct sockaddr_in addr, si_other;
    int slen, recv_len;
    char buffer[512];
    ClientInfo_t clients[MAX_CLIENTS];
    int num_clients;
} ServerData_t;

typedef struct ClientData_t {
    #ifdef _WIN32
        WSADATA wsa;
        SOCKET socket;
    #else
        int socket;
    #endif
    struct sockaddr_in si_other;
    int slen, recv_len;
    char buffer[512];
    int uid;
    PlayerInfo_t players[MAX_CLIENTS];
    int num_players;
    
    float ka_timer;
    float ka_timeout;
} ClientData_t;

typedef struct Network_t {
    bool server_running;
    bool client_running;
    ServerData_t server;
    ClientData_t client;
} Network_t;

Network_t network;

bool initNetwork();
void freeNetwork();

bool startServerNetwork(int);
bool startClientNetwork(const char*, int);

void updateServerNetwork();
void updateClientNetwork();

bool sendDataClient(void*, int, int);
bool sendDataServer(void*, int, int, int);

void drawPlayers();
void drawPlayerList();

#endif
