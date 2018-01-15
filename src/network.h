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
// #include <SDL2/SDL_net.h>
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

typedef struct Network_t {
    // UDPsocket server_socket;
    // UDPpacket server_packet;
    // IPaddress socket_address;
    
    // IPaddress server_address;
    // UDPsocket client_socket;
    // UDPpacket client_packet;
} Network_t;

static Network_t* network;

bool initNetwork();
void freeNetwork();
bool startServerNetwork(int);
bool startClientNetwork(const char*, int);
void updateServerNetwork();
void updateClientNetwork();

#endif
