#ifndef CLIENT_PACKETS_H
#define CLIENT_PACKETS_H

typedef struct ClientInfoPacket_t {
    char player_name[32];
} ClientInfoPacket_t;

typedef struct ClientConnectPacket_t {
    int buffer;
} ClientConnectPacket_t;

#endif
