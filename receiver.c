#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>
#include <sys/time.h>

#pragma comment(lib, "ws2_32.lib")  // Link Winsock library

#define DEFAULT_PORT 8080

void error_exit(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

// Function to get the current time in high resolution
LARGE_INTEGER get_time() {
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    return time;
}

// Function to calculate the time difference in microseconds
double time_diff_us(LARGE_INTEGER start, LARGE_INTEGER end, LARGE_INTEGER freq) {
    return (double)(end.QuadPart - start.QuadPart) * 1000000.0 / freq.QuadPart;
}

// struct timeval GetTimeStamp() {
//     struct timeval tv;
//     gettimeofday(&tv,NULL);
//     return tv;
// }

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <output text file>\n", argv[0]);
        return 1;
    }

    const char *output_file = argv[1];
    LARGE_INTEGER start_time, end_time, frequency;
    // Get the frequency of the high-resolution performance counter
    QueryPerformanceFrequency(&frequency);
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

    // Set up local address to bind to
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DEFAULT_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        error_exit("Bind failed");
    }

    FILE *file = fopen(output_file, "w");
    if (!file) {
        closesocket(sock);
        WSACleanup();
        error_exit("Failed to open output file");
    }

    char buffer[1024];
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    clock_t t1, t2;

    // Receive packets and calculate spacing between pairs
    for (int i = 0; i < 100; i++) {
        
        if (recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_addr_len) == SOCKET_ERROR) {
            printf("Failed to receive packet\n");
            continue;
        }
        struct timeval tv;
        gettimeofday(&tv,NULL);
        unsigned long start_time_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
        printf("Received first packet starting timer\n");
        // start_time = get_time();
        if (recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_addr_len) == SOCKET_ERROR) {
            printf("Failed to receive packet\n");
            continue;
        }
        // end_time = get_time();
        gettimeofday(&tv,NULL);
        unsigned long end_time_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
        printf("Received second packet stopped timer\n");
        unsigned long time_diff_microsec = end_time_in_micros-start_time_in_micros;

        int packet_id;
        sscanf(buffer, "Packet ID: %d", &packet_id);  // Extract the packet ID from the buffer
        // Print the packet ID and time difference
        printf("Received packet ID: %d\n", packet_id);

        fprintf(file, "P/(t2 - t1) = %f Mbps\n", 8000 / double(time_diff_microsec));
    }

    fclose(file);
    closesocket(sock);
    WSACleanup();
    return 0;
}
