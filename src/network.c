#include "network.h"

bool initNetwork()
{
    network = (Network_t*)malloc(sizeof(Network_t));
    network->server_running = false;
    network->client_running = false;
    // network->slen = sizeof(network->si_other);
    // network->server_socket = NULL;
    
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
    if (ioctlsocket(network->server.socket, FIONBIO, &nmode) == SOCKET_ERROR) {
        printf("[NET][CLIENT] Failed to set nonblocking mode\n");
        return true;
    }
    printf("[NET][CLIENT] Enabled nonblocking mode\n");
    
    if (sendto(network->client.socket, "SHIT", 4, 0, &network->client.si_other, network->client.slen) == SOCKET_ERROR) {
        printf("[NET][CLIENT] Failed to send test packet\n");
    }
    printf("[NET][CLIENT] Sent test packet\n");
    
    network->client_running = true;
    
    return false;
}

void updateServerNetwork()
{
    if (!network->server_running) return;
    
    if ((network->server.recv_len = recvfrom(network->server.socket, network->server.buffer, 512, 0, &network->server.si_other, &network->server.slen)) != SOCKET_ERROR) {
        printf("Received data from: %s:%d\n", inet_ntoa(network->server.si_other.sin_addr), ntohs(network->server.si_other.sin_port));
        printf("Data: %s\n", network->server.buffer);
        pushlineOS(local_os, FormatText("Server received data: %s", network->server.buffer));
        memset(network->server.buffer, '\0', 512);
    }
    
}

void updateClientNetwork()
{
}
