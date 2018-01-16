#ifndef SERVER_PACKETS_H
#define SERVER_PACKETS_H

typedef struct UIDPacket_t {
    int uid;
} UIDPacket_t;

typedef struct NewPlayerPacket_t {
    int uid;
    float x;
    float z;
} NewPlayerPacket_t;

#endif
