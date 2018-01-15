#include "network.h"

bool initNetwork()
{
    network = (Network_t*)malloc(sizeof(Network_t));
    // network->server_socket = NULL;
    // network->client_socket = NULL;
    
    // if (SDLNet_Init() != 0) {
    //     printf("[NET] Failed to init SDLNet\n");
    //     return true;
    // }
    return false;
}

void freeNetwork()
{
    // if (network->server_socket != NULL) {
    //     SDLNet_UDP_Close(network->server_socket);
    // }
    // if (network->client_socket != NULL) {
    //     SDLNet_UDP_Close(network->client_socket);
    // }
    
    // free(network);
}

bool startServerNetwork(int port)
{
    // network->server_socket = SDLNet_UDP_Open(port);
    // if (network->server_socket == NULL) {
    //     printf("[NET] Failed to create server socket\n");
    //     return true;
    // }
    // if (SDLNet_ResolveHost(&network->socket_address, NULL, port) != 0) {
    //     printf("[NET] Failed to resolve host\n");
    //     return true;
    // }
    // if (SDLNet_UDP_Bind(network->server_socket, 0, &network->socket_address) == -1) {
    //     printf("[NET] Failed to bind socket\n");
    //     return true;
    // }
    
    return false;
}

bool startClientNetwork(const char* ip, int port)
{
    // network->client_socket = SDLNet_UDP_Open(0);
    // if (network->client_socket == NULL) {
    //     printf("[NET] Failed to create client socket\n");
    //     return true;
    // }
    // if (SDLNet_ResolveHost(&network->server_address, ip, port) != 0) {
    //     printf("[NET] Failed to resolve host\n");
    //     return true;
    // }
    // if (SDLNet_UDP_Bind(network->client_socket, -1, &network->server_address) == -1) {
    //     printf("[NET] Failed to bind socket\n");
    //     return true;
    // }
    
    // static char* d = "This is some bullshit";
    // UDPpacket* packet = SDLNet_AllocPacket(1024);
    // packet->len = strlen(d)+1;
    // memcpy(packet->data, d, packet->len);
    // if (SDLNet_UDP_Send(network->client_socket, packet->channel, packet) == 0) {
    //     printf("[NET][CLIENT] Failed to send packet: %s\n", SDLNet_GetError());
    // }
    // SDLNet_FreePacket(packet);
    
    return false;
}

void updateServerNetwork()
{
    // if (network == NULL) return;
    // if (network->server_socket == NULL) return;
    
    // if (SDLNet_UDP_Recv(network->server_socket, &network->server_packet)) {
    //     printf("[NET][SERVER] Received packet\n");
    // }
    
}

void updateClientNetwork()
{
    // if (network == NULL) return;
    // if (network->client_socket == NULL) return;

    // if (SDLNet_UDP_Recv(network->client_socket, &network->client_packet)) {
    //     printf("[NET][CLIENT] Received packet\n");
    // }

}
