#include "network.h"

bool initNetwork()
{
    // network = (Network_t*)malloc(sizeof(Network_t));
    network.server_running = false;
    network.client_running = false;
    
    return false;
}

void freeNetwork()
{
    #ifdef _WIN32  
        closesocket(network.server.socket);
        closesocket(network.client.socket);
        WSACleanup();
    #else
        close(network.server.socket);
        close(network.client.socket);
    #endif
}

bool startServerNetwork(int port)
{
    if (network.server_running) return true;
    
    memset(network.server.buffer, '\0', 512);
    network.server.num_clients = 0;
    network.server.slen = sizeof(network.server.si_other);
    
    #ifdef _WIN32
        if (WSAStartup(MAKEWORD(2,2),&network.server.wsa) != 0) {
            printf("[NET][SERVER] WSAStartup failed. (%d)\n", HACK_GETERROR());
            return true;
        }
        printf("[NET][SERVER] WSAStartup succeeded\n");
    #endif
    
    if ((network.server.socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("[NET][SERVER] Could not create socket. (%d)\n", HACK_GETERROR());
        return true;
    }
    printf("[NET][SERVER] Created server socket\n");
    
    network.server.addr.sin_family = AF_INET;
    network.server.addr.sin_addr.s_addr = INADDR_ANY;
    network.server.addr.sin_port = htons(port);
    
    int nmode = 1;
    if (ioctlsocket(network.server.socket, FIONBIO, &nmode) == SOCKET_ERROR) {
        printf("[NET][SERVER] Failed to set nonblocking mode\n");
        return true;
    }
    printf("[NET][SERVER] Enabled nonblocking mode\n");
    
    if (bind(network.server.socket, (struct sockaddr_in*)&network.server.addr, sizeof(network.server.addr)) == SOCKET_ERROR) {
        printf("[NET][SERVER] Failed to bind socket\n");
        return true;
    }
    printf("[NET][SERVER] Bound server socket\n");
    
    network.server_running = true;
    
    return false;
}

bool startClientNetwork(const char* ip, int port)
{
    if (network.client_running) return true;
    
    memset(network.client.buffer, '\0', 512);
    network.client.slen = sizeof(network.client.si_other);
    network.client.num_players = 0;
    
    #ifdef _WIN32
        if (WSAStartup(MAKEWORD(2,2),&network.client.wsa) != 0) {
            printf("[NET][CLIENT] WSAStartup failed. (%d)\n", HACK_GETERROR());
            return true;
        }
        printf("[NET][CLIENT] WSAStartup succeeded\n");
    #endif
    
    if ((network.client.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
        printf("[NET][CLIENT] Could not create socket. (%d)\n", HACK_GETERROR());
        return true;
    }
    printf("[NET][CLIENT] Created client socket\n");
    
    network.client.si_other.sin_family = AF_INET;
    network.client.si_other.sin_port = htons(port);
    #ifdef _WIN32
        network.client.si_other.sin_addr.S_un.S_addr = inet_addr(ip);
    #else
        network.client.si_other.sin_addr.s_addr = inet_addr(ip);
    #endif
    
    int nmode = 1;
    if (ioctlsocket(network.client.socket, FIONBIO, &nmode) == SOCKET_ERROR) {
        printf("[NET][CLIENT] Failed to set nonblocking mode\n");
        return true;
    }
    printf("[NET][CLIENT] Enabled nonblocking mode\n");
    
    network.client_running = true;
    
    ClientConnectPacket_t* conn_packet = (ClientConnectPacket_t*)malloc(sizeof(conn_packet));
    // conn_packet->buffer = -1;
    conn_packet->x = local_screen.pos.x;
    conn_packet->z = local_screen.pos.z;
    sendDataClient(conn_packet, sizeof(ClientConnectPacket_t), PACKET_CONNECT);
    free(conn_packet);
    
    return false;
}

void updateServerNetwork()
{
    if (!network.server_running) return;
    // printf("updating server\n");
    
    if ((network.server.recv_len = recvfrom(network.server.socket, network.server.buffer, 512, 0, &network.server.si_other, &network.server.slen)) != SOCKET_ERROR) {
        int packet_type = network.server.buffer[network.server.recv_len-1];
        switch (packet_type)
        {
            case PACKET_TEST: {
                TestPacket_t* packet = (TestPacket_t*)malloc(sizeof(TestPacket_t));
                memcpy(packet, network.server.buffer, network.server.recv_len-1);
                pushlineOS(&local_os, FormatText("received test packet: %d:%s:%f:%f\n", packet->test1, packet->test2, packet->test3, packet->test4));
                free(packet);
            } break;
            case PACKET_CONNECT: {
                if (network.server.num_clients < MAX_CLIENTS) {
                    ClientConnectPacket_t* packet = (ClientConnectPacket_t*)malloc(sizeof(ClientConnectPacket_t));
                    memcpy(packet, network.server.buffer, network.server.recv_len-1);
                    
                    printf("[NET][SERVER] Connect packet received.. Adding client.\n");
                    network.server.clients[network.server.num_clients].si_other = network.server.si_other;
                    network.server.clients[network.server.num_clients].uid = network.server.num_clients;
                    network.server.clients[network.server.num_clients].slen = network.server.slen;
                    network.server.clients[network.server.num_clients].x = packet->x;
                    network.server.clients[network.server.num_clients].z = packet->z;
                    network.server.clients[network.server.num_clients].rx = 0;
                    network.server.clients[network.server.num_clients].rz = 0;
                    network.server.clients[network.server.num_clients].rot = 0;
                    network.server.num_clients++;
                    
                    UIDPacket_t* uid_packet = (UIDPacket_t*)malloc(sizeof(UIDPacket_t));
                    uid_packet->uid = network.server.num_clients-1;
                    sendDataServer(uid_packet, sizeof(UIDPacket_t), PACKET_UID, uid_packet->uid);
                    
                    NewPlayerPacket_t* player_packet = (NewPlayerPacket_t*)malloc(sizeof(NewPlayerPacket_t));
                    player_packet->uid = uid_packet->uid;
                    player_packet->x = packet->x;
                    player_packet->z = packet->z;
                    sendDataServer(player_packet, sizeof(NewPlayerPacket_t), PACKET_NEWPLAYER, -1);
                    
                    for (int i = 0; i < network.server.num_clients; i++) {
                        if (network.server.clients[i].uid == uid_packet->uid) continue;
                        NewPlayerPacket_t* player_packet = (NewPlayerPacket_t*)malloc(sizeof(NewPlayerPacket_t));
                        player_packet->uid = network.server.clients[i].uid;
                        player_packet->x = network.server.clients[i].x;
                        player_packet->z = network.server.clients[i].z;
                        sendDataServer(player_packet, sizeof(NewPlayerPacket_t), PACKET_NEWPLAYER, uid_packet->uid);
                        free(player_packet);
                    }
                    
                    
                    free(uid_packet);
                    free(player_packet);
                    free(packet);
                }
            } break;
            case PACKET_POSITION: {
                PlayerPositionPacket_t* packet = (PlayerPositionPacket_t*)malloc(sizeof(PlayerPositionPacket_t));
                memcpy(packet, network.server.buffer, network.server.recv_len-1);
                for (int i = 0; i < network.server.num_clients; i++) {
                    sendDataServer(packet, sizeof(PlayerPositionPacket_t), PACKET_POSITION, network.server.clients[i].uid);
                }
                free(packet);
            } break;
            default: {
                printf("[NET][SERVER] Packet type not handled: %d\n", packet_type);
            } break;
        }
        memset(network.server.buffer, '\0', 512);
    }
}

void updateClientNetwork()
{
    if (!network.client_running) return;
    
    if ((network.client.recv_len = recvfrom(network.client.socket, network.client.buffer, 512, 0, &network.client.si_other, &network.client.slen)) != SOCKET_ERROR) {
        int packet_type = network.client.buffer[network.client.recv_len-1];
        switch (packet_type)
        {
            case PACKET_UID: {
                UIDPacket_t* packet = (UIDPacket_t*)malloc(sizeof(UIDPacket_t));
                memcpy(packet, network.client.buffer, network.client.recv_len-1);
                network.client.uid = packet->uid;
                pushlineOS(&local_os, FormatText("Client got uid: %d", packet->uid));
                free(packet);
            } break;
            case PACKET_NEWPLAYER: {
                NewPlayerPacket_t* packet = (NewPlayerPacket_t*)malloc(sizeof(NewPlayerPacket_t));
                memcpy(packet, network.client.buffer, network.client.recv_len-1);
                if (network.client.num_players < MAX_CLIENTS) {
                    printf("[NET][CLIENT] Creating new player desk with id %d at %fx %fz\n", packet->uid, packet->x, packet->z);
                    network.client.players[network.client.num_players].uid = packet->uid;
                    network.client.players[network.client.num_players].x = packet->x;
                    network.client.players[network.client.num_players].z = packet->z;
                    network.client.players[network.client.num_players].rx = 0;
                    network.client.players[network.client.num_players].rz = 0;
                    network.client.players[network.client.num_players].rot = 0;
                    network.client.players[network.client.num_players].screen.texture = LoadRenderTexture(screen_w*screen_w_gl, screen_h*screen_h_gl);
                    network.client.num_players++;
                }
                free(packet);
            } break;
            case PACKET_POSITION: {
                PlayerPositionPacket_t* packet = (PlayerPositionPacket_t*)malloc(sizeof(PlayerPositionPacket_t));
                memcpy(packet, network.client.buffer, network.client.recv_len-1);
                for (int i = 0; i < network.client.num_players; i++) {
                    if (network.client.players[i].uid == packet->uid) {
                        network.client.players[i].rx = packet->x;
                        network.client.players[i].rz = packet->z;
                        network.client.players[i].rot = packet->rot;
                        break;
                    }
                }
                free(packet);
            } break;
            default: {
                printf("[NET][CLIENT] Packet type not handled: %d\n", packet_type);
            } break;
        }
        memset(network.client.buffer, '\0', 512);
    }
}

bool sendDataClient(void* data, int size, int type)
{
    if (!network.client_running) return true;
    
    void* send = (void*)malloc(size+1);
    memcpy(send, data, size);
    memcpy(send+size, &type, 1);
    if (sendto(network.client.socket, send, size+1, 0, &network.client.si_other, network.client.slen) == SOCKET_ERROR) {
        printf("[NET][CLIENT] Failed to send packet\n");
        free(send);
        return true;
    }
    free(send);
    
    return false;
}

bool sendDataServer(void* data, int size, int type, int to)
{
    if (!network.server_running) return true;
    
    void* send = (void*)malloc(size+1);
    memcpy(send, data, size);
    memcpy(send+size, &type, 1);
    if (to > -1) {
        ClientInfo_t* rec = NULL;
        for (int i = 0; i < network.server.num_clients; i++) {
            if (network.server.clients[i].uid == to) {
                rec = &network.server.clients[i];
                break;
            }
        }
        if (rec != NULL) {
            if (sendto(network.server.socket, send, size+1, 0, &rec->si_other, rec->slen) == SOCKET_ERROR) {
                printf("[NET][SERVER] Failed to send packet\n");
                free(send);
                return true;
            }
        }
    } else {
        for (int i = 0; i < network.server.num_clients; i++) {
            ClientInfo_t* rec = &network.server.clients[i];
            if (sendto(network.server.socket, send, size+1, 0, &rec->si_other, rec->slen) == SOCKET_ERROR) {
                printf("[NET][SERVER] Failed to send packet\n");
                free(send);
                return true;
            }
        }
    }
    free(send);
    return false;
}

void drawPlayers()
{
    if (!network.client_running) return;
    
    for (int i = 0; i < network.client.num_players; i++)
    {
        network.client.players[i].screen.pos.x = network.client.players[i].x;
        network.client.players[i].screen.pos.y = 0;
        network.client.players[i].screen.pos.z = network.client.players[i].z;
        network.client.players[i].screen.realpos.x = network.client.players[i].rx;
        network.client.players[i].screen.realpos.y = 0;
        network.client.players[i].screen.realpos.z = network.client.players[i].rz;
        if (network.client.players[i].uid != network.client.uid) {
            drawScreen(&(network.client.players[i].screen));
            drawPlayer(&(network.client.players[i].screen), -network.client.players[i].rot);
        }
    }
}

void drawPlayerList()
{
     if (!network.client_running) return;
    
    for (int i = 0; i < network.client.num_players; i++)
    {
        DrawText(FormatText("Player (%d:%d): %f : %f", i, network.client.players[i].uid, network.client.players[i].screen.pos.x, network.client.players[i].screen.pos.z), 10, 50+i*15, 10, WHITE);
    }
}
