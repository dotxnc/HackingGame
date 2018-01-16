#include "network.h"

bool initNetwork()
{
    network = (Network_t*)malloc(sizeof(Network_t));
    network->server_running = false;
    network->client_running = false;
    
    return false;
}

void freeNetwork()
{
    closesocket(network->server.socket);
    closesocket(network->client.socket);
    WSACleanup();
}

bool startServerNetwork(int port)
{
    memset(network->server.buffer, '\0', 512);
    network->server.num_clients = 0;
    
    if (WSAStartup(MAKEWORD(2,2),&network->server.wsa) != 0) {
        printf("[NET][SERVER] WSAStartup failed. (%d)\n", WSAGetLastError());
        return true;
    }
    printf("[NET][SERVER] WSAStartup succeeded\n");
    
    if ((network->server.socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("[NET][SERVER] Could not create socket. (%d)\n", WSAGetLastError());
        return true;
    }
    printf("[NET][SERVER] Created server socket\n");
    
    network->server.addr.sin_family = AF_INET;
    network->server.addr.sin_addr.s_addr = INADDR_ANY;
    network->server.addr.sin_port = htons(port);
    
    int nmode = 1;
    if (ioctlsocket(network->server.socket, FIONBIO, &nmode) == SOCKET_ERROR) {
        printf("[NET][SERVER] Failed to set nonblocking mode\n");
        return true;
    }
    printf("[NET][SERVER] Enabled nonblocking mode\n");
    
    if (bind(network->server.socket, (struct sockaddr_in*)&network->server.addr, sizeof(network->server.addr)) == SOCKET_ERROR) {
        printf("[NET][SERVER] Failed to bind socket\n");
        return true;
    }
    printf("[NET][SERVER] Bound server socket\n");
    
    network->server_running = true;
    
    return false;
}

bool startClientNetwork(const char* ip, int port)
{
    memset(network->client.buffer, '\0', 512);
    network->client.slen = sizeof(network->client.si_other);
    
    if (WSAStartup(MAKEWORD(2,2),&network->server.wsa) != 0) {
        printf("[NET][CLIENT] WSAStartup failed. (%d)\n", WSAGetLastError());
        return true;
    }
    printf("[NET][CLIENT] WSAStartup succeeded\n");
    
    if ((network->client.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
        printf("[NET][CLIENT] Could not create socket. (%d)\n", WSAGetLastError());
        return true;
    }
    printf("[NET][CLIENT] Created client socket\n");
    
    network->client.si_other.sin_family = AF_INET;
    network->client.si_other.sin_port = htons(port);
    network->client.si_other.sin_addr.S_un.S_addr = inet_addr(ip);
    
    int nmode = 1;
    if (ioctlsocket(network->client.socket, FIONBIO, &nmode) == SOCKET_ERROR) {
        printf("[NET][CLIENT] Failed to set nonblocking mode\n");
        return true;
    }
    printf("[NET][CLIENT] Enabled nonblocking mode\n");
    
    network->client_running = true;
    
    ClientConnectPacket_t* conn_packet = (ClientConnectPacket_t*)malloc(sizeof(conn_packet));
    conn_packet->buffer = -1;
    sendDataClient(conn_packet, sizeof(conn_packet), PACKET_CONNECT);
    
    return false;
}

void updateServerNetwork()
{
    if (!network->server_running) return;
    
    if ((network->server.recv_len = recvfrom(network->server.socket, network->server.buffer, 512, 0, &network->server.si_other, &network->server.slen)) != SOCKET_ERROR) {
        int packet_type = network->server.buffer[network->server.recv_len-1];
        switch (packet_type)
        {
            case PACKET_TEST: {
                TestPacket_t* packet = (TestPacket_t*)malloc(sizeof(packet));
                memcpy(packet, network->server.buffer, network->server.recv_len-1);
                pushlineOS(local_os, FormatText("received test packet: %d:%s:%f:%f\n", packet->test1, packet->test2, packet->test3, packet->test4));
                free(packet);
            } break;
            case PACKET_CONNECT: {
                if (network->server.num_clients < MAX_CLIENTS) {
                    printf("[NET][SERVER] Connect packet received.. Adding client.");
                    network->server.clients[network->server.num_clients].si_other = network->server.si_other;
                    network->server.clients[network->server.num_clients].uid = network->server.num_clients;
                    network->server.clients[network->server.num_clients].slen = network->server.slen;
                    network->server.num_clients++;
                    
                    UIDPacket_t* uid_packet = (UIDPacket_t*)malloc(sizeof(uid_packet));
                    uid_packet->uid = network->server.num_clients-1;
                    sendDataServer(uid_packet, sizeof(uid_packet), PACKET_UID, uid_packet->uid);
                }
            } break;
            default: {
                printf("[NET][SERVER] Packet type not handled: %d\n", packet_type);
            } break;
        }
        memset(network->server.buffer, '\0', 512);
    }
}

void updateClientNetwork()
{
    if (!network->client_running) return;
    
    if ((network->client.recv_len = recvfrom(network->client.socket, network->client.buffer, 512, 0, &network->client.si_other, &network->client.slen)) != SOCKET_ERROR) {
        int packet_type = network->client.buffer[network->client.recv_len-1];
        switch (packet_type)
        {
            case PACKET_UID: {
                UIDPacket_t* packet = (UIDPacket_t*)malloc(sizeof(packet));
                memcpy(packet, network->client.buffer, network->client.recv_len-1);
                network->client.uid = packet->uid;
                pushlineOS(local_os, FormatText("Client got uid: %d", packet->uid));
                free(packet);
            } break;
            default: {
                printf("[NET][CLIENT] Packet type not handled: %d\n", packet_type);
            } break;
        }
        memset(network->client.buffer, '\0', 512);
    }
}

bool sendDataClient(void* data, int size, int type)
{
    if (!network->client_running) return true;
    
    void* send = (void*)malloc(size+1);
    memcpy(send, data, size);
    memcpy(send+size, &type, 1);
    if (sendto(network->client.socket, send, size+1, 0, &network->client.si_other, network->client.slen) == SOCKET_ERROR) {
        printf("[NET][CLIENT] Failed to send packet\n");
        free(send);
        return true;
    }
    free(send);
    
    return false;
}

bool sendDataServer(void* data, int size, int type, int to)
{
    if (!network->server_running) return true;
    
    void* send = (void*)malloc(size+1);
    memcpy(send, data, size);
    memcpy(send+size, &type, 1);
    if (to > -1) {
        ClientInfo_t* rec = NULL;
        for (int i = 0; i < network->server.num_clients; i++) {
            if (network->server.clients[i].uid == to) {
                rec = &network->server.clients[i];
                break;
            }
        }
        if (rec != NULL) {
            if (sendto(network->server.socket, send, size+1, 0, &rec->si_other, rec->slen) == SOCKET_ERROR) {
                printf("[NET][SERVER] Failed to send packet\n");
                free(send);
                return true;
            }
        }
    } else {
        for (int i = 0; i < network->server.num_clients; i++) {
            ClientInfo_t rec = network->server.clients[i];
            if (sendto(network->server.socket, send, size+1, 0, &rec.si_other, rec.slen) == SOCKET_ERROR) {
                printf("[NET][SERVER] Failed to send packet\n");
                free(send);
                return true;
            }
        }
    }
    free(send);
    return false;
}
