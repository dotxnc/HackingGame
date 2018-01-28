#ifndef CLIENT_PACKETS_H
#define CLIENT_PACKETS_H

#define MAX_INPUT 45

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

#endif
