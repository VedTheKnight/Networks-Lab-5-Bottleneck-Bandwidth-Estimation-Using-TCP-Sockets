// References : ChatGPT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

long get_current_time() {
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    return (time_now.tv_sec * 1000000) + time_now.tv_usec;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <output file>\n", argv[0]);
        return 1;
    }

    FILE *output_file = fopen(argv[1], "w");
    if (!output_file) {
        perror("Could not open output file");
        return 1;
    }

    int sockfd;
    struct sockaddr_in server_addr, sender_addr;
    char buffer[1024];
    socklen_t addr_len = sizeof(sender_addr);
    long recv_time_1, recv_time_2;
    int packet_count = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8081);  // Use the same port number as the sender
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // Receive packets in pairs
    while (1) {
        // Read data from the socket
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender_addr, &addr_len);
        if (n < 0) {
            perror("recvfrom failed");
            break;
        }

        recv_time_1 = get_current_time();
        packet_count++;

        // Receive the second packet of the pair
        n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender_addr, &addr_len);
        if (n < 0) {
            perror("recvfrom failed");
            break;
        }

        recv_time_2 = get_current_time();
        packet_count++;
        // Extract the packet ID (assuming it's at the start of the buffer)
        int packet_id;
        if (sscanf(buffer, "Packet %d", &packet_id) == 1){
            int packet_id_temp;
            if(packet_id == 0)
                packet_id_temp = packet_count;
            else
                packet_id_temp = packet_id;
            printf("Received packet pair number : %d\n", packet_id_temp/2);
        }
        else{
            printf("Received packet with unknown format\n");
        }

        // Measure the time of arrival of packets and compute P/(t2 - t1)
        long time_difference = recv_time_2 - recv_time_1;
        fprintf(output_file, "P/(t1-t2) = %f Mbps\n", 8000.0/time_difference); // Log the bit rate
        //printf("P/(t1-t2) = %f Mbps\n", 8000.0/time_difference); // Log the bit rate

        if(packet_id == 0){
           break;
        }
    }

    fclose(output_file);
    close(sockfd);
    return 0;
}