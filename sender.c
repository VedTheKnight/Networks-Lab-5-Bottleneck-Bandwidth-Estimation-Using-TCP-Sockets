#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")  // Link Winsock library

#define DEFAULT_PORT 8080

void error_exit(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <packet size in bits> <destination IP> <spacing (ms)> <number of packet pairs>\n", argv[0]);
        return 1;
    }

    int packet_size_bits = atoi(argv[1]);
    char *dest_ip = argv[2];
    int spacing_ms = atoi(argv[3]);
    int num_packet_pairs = atoi(argv[4]);
    int packet_size_bytes = packet_size_bits / 8;

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        error_exit("WSAStartup failed");
    }

    // Create socket
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        error_exit("Socket creation failed");
    }

    // Set up destination address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, dest_ip, &server_addr.sin_addr);

    // Send packets
    char *packet = (char *)malloc(packet_size_bytes);
    if (!packet) {
        error_exit("Memory allocation failed");
    }

    for (int i = 0; i < num_packet_pairs; i++) {
        sprintf(packet, "Packet #%d", i + 1);

        // Send first packet of the pair
        if (sendto(sock, packet, packet_size_bytes, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            printf("Failed to send packet %d\n", i + 1);
        }

        // Send second packet immediately after
        if (sendto(sock, packet, packet_size_bytes, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            printf("Failed to send packet %d\n", i + 1);
        }

        // Wait for the specified spacing before sending the next pair
        Sleep(spacing_ms);
    }

    // Clean up
    free(packet);
    closesocket(sock);
    WSACleanup();
    return 0;
}
