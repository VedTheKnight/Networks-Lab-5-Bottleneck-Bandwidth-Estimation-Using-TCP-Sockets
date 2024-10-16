// References : ChatGPT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

// Function to get the current time in microseconds
long get_current_time() {
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    return (time_now.tv_sec * 1000000) + time_now.tv_usec;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <packet size in bits> <destination IP> <spacing (ms)> <total pairs>\n", argv[0]);
        return 1;
    }

    int packet_size = atoi(argv[1]) / 8; // Convert size from bits to bytes
    char *dest_ip = argv[2];
    int spacing_ms = atoi(argv[3]);
    int total_pairs = atoi(argv[4]);

    int sockfd;
    struct sockaddr_in receiver_addr;
    char *packet;
    long start_time, send_time;

    // Create Datagram Socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // AF_INET = IPv4, SOCK_DGRAM = UDP
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&receiver_addr, 0, sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(8081); // Use any port number (you can change this)
    receiver_addr.sin_addr.s_addr = inet_addr(dest_ip);

    // Allocate memory for the packet
    packet = malloc(packet_size);

    // Sending packets in pairs
    for (int i = 1; i <= total_pairs * 2; i++) {
        memset(packet, 0, packet_size);

        sprintf(packet, "Packet %d", i%(2*total_pairs));  // Include packet number in the data


        // Send/write data to the socket
        send_time = get_current_time();
        if (sendto(sockfd, packet, packet_size, 0, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) < 0) {
            perror("sendto failed");
            exit(EXIT_FAILURE);
        }

        // For odd packets (first in a pair), send the next packet immediately
        if (i % 2 == 0) {
            usleep(spacing_ms * 1000); // Wait between pairs
        }
    }

    free(packet);
    close(sockfd);
    return 0;
}