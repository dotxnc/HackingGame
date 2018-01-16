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
#include "clientpackets.h"
#include "serverpackets.h"

#define MAX_CLIENTS 8

typedef enum PACKET_TYPE {
    PACKET_TEST=0,
    PACKET_CONNECT,
    PACKET_DISCONNECT,
    PACKET_POSITON,
    PACKET_CHAT,
    PACKET_UID
} PACKET_TYPE;

// shared packets
typedef struct TestPacket_t {
    int test1;
    char test2[32];
    double test3;
    float test4;
} TestPacket_t;
// shared packets

typedef struct ClientInfo_t {
    int uid;
    int slen;
    struct sockaddr_in si_other;
} ClientInfo_t;

typedef struct ServerData_t {
    SOCKET socket;
    struct sockaddr_in addr, si_other;
    int slen, recv_len;
    char buffer[512];
    WSADATA wsa;
    ClientInfo_t clients[MAX_CLIENTS];
    int num_clients;
} ServerData_t;

typedef struct ClientData_t {
    SOCKET socket;
    struct sockaddr_in si_other;
    int slen, recv_len;
    char buffer[512];
    WSADATA wsa;
    int uid;
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

bool sendDataClient(void*, int, int);
bool sendDataServer(void*, int, int, int);

#endif
