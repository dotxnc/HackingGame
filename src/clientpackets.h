#ifndef CLIENT_PACKETS_H
#define CLIENT_PACKETS_H

typedef struct ClientInfoPacket_t {
    char player_name[32];
} ClientInfoPacket_t;

typedef struct ClientConnectPacket_t {
    float x;
    float z;
} ClientConnectPacket_t;

typedef struct PlayerPositionPacket_t {
    float uid;
    float x;
    float z;
    float rot;
} PlayerPositionPacket_t;

#endif
