// client.c
#include "networking.h"

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Failed to create socket");
        return -1;
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "34.67.134.34", &server_addr.sin_addr); // Replace with your server's IP

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to connect to server");
        close_connection(client_socket);
        return -1;
    }
    
    printf("Connected to server successfully!\n"); // Added line for debugging

    send_message(client_socket, "Hello from client!");
    char *response = receive_message(client_socket);
    printf("Received: %s\n", response);
    free(response);

    while(1){};

    //close_connection(client_socket);
    return 0;
}
