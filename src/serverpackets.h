#ifndef SERVER_PACKETS_H
#define SERVER_PACKETS_H

typedef struct UIDPacket_t {
    int uid;
} UIDPacket_t;

typedef struct NewPlayerPacket_t {
    int uid;
} NewPlayerPacket_t;

#endif
