#ifndef PACKETS_H
#define PACKETS_H
#include "os.h"

typedef enum PACKET_TYPE {
    PACKET_TEST=0,
    PACKET_CONNECT,
    PACKET_DISCONNECT,
    PACKET_POSITION,
    PACKET_CHAT,
    PACKET_UID,
    PACKET_NEWPLAYER,
    PACKET_KEEPALIVE
} PACKET_TYPE;

typedef struct TestPacket_t {
    int test1;
    char test2[32];
    double test3;
    float test4;
} TestPacket_t;

typedef struct UIDPacket_t {
    int uid;
} UIDPacket_t;

typedef struct NewPlayerPacket_t {
    int uid;
    float x;
    float z;
} NewPlayerPacket_t;

typedef struct ClientInfoPacket_t {
    char player_name[32];
} ClientInfoPacket_t;

typedef struct ClientConnectPacket_t {
    float x;
    float z;
} ClientConnectPacket_t;

typedef struct PlayerPositionPacket_t {
    int uid;
    float x;
    float z;
    float rot;
} PlayerPositionPacket_t;

typedef struct ChatPacket_t {
    int uid;
    char chat[MAX_INPUT];
} ChatPacket_t;

typedef struct KeepAlivePacket_t {
    int uid;
} KeepAlivePacket_t;

#endif
