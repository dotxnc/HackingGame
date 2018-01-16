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
    if (ioctlsocket(network->client.socket, FIONBIO, &nmode) == SOCKET_ERROR) {
        printf("[NET][CLIENT] Failed to set nonblocking mode\n");
        return true;
    }
    printf("[NET][CLIENT] Enabled nonblocking mode\n");
    
    network->client_running = true;
    
    TestPacket_t* test_packet = (TestPacket_t*)malloc(sizeof(test_packet));
    test_packet->test1 = 16;
    strcpy(test_packet->test2, "This is a test packet");
    test_packet->test3 = 3.5;
    test_packet->test4 = 64.56432f;
    
    sendDataClient(test_packet, sizeof(TestPacket_t), PACKET_TEST);
    
    return false;
}

void updateServerNetwork()
{
    if (!network->server_running) return;
    
    if ((network->server.recv_len = recvfrom(network->server.socket, network->server.buffer, 512, 0, &network->server.si_other, &network->server.slen)) != SOCKET_ERROR) {
        int packet_type = network->server.buffer[network->server.recv_len-1];
        if (packet_type == PACKET_TEST) {
            TestPacket_t* packet = (TestPacket_t*)malloc(sizeof(packet));
            memcpy(packet, network->server.buffer, network->server.recv_len-1);
            
            pushlineOS(local_os, FormatText("received test packet: %d:%s:%f:%f\n", packet->test1, packet->test2, packet->test3, packet->test4));
        }
        
        memset(network->server.buffer, '\0', 512);
    }
}

void updateClientNetwork()
{
    if (!network->client_running) return;
    
    if ((network->client.recv_len = recvfrom(network->client.socket, network->client.buffer, 512, 0, &network->client.si_other, &network->client.slen)) != SOCKET_ERROR) {
        memset(network->client.buffer, '\0', 512);
    }
}

bool sendDataClient(void* data, int size, int type)
{
    if (!network->client_running) return true;
    
    void* send = (void*)malloc(size+1);
    memcpy(send, data, size);
    memcpy(send+size, &type, 1);
    printf("send data: %s:%d:%d\n", send, size);
    if (sendto(network->client.socket, send, size+1, 0, &network->client.si_other, network->client.slen) == SOCKET_ERROR) {
        printf("[NET][CLIENT] Failed to send packet\n");
        free(send);
        return true;
    }
    free(send);
    
    return false;
}

bool sendDataServer(void* data, int size, int type)
{
    if (!network->server_running) return true;
    return false;
}
