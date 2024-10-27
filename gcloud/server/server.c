// server.c
#include "networking.h"

int main() {
    int server_socket = create_server(8080);
    printf("Server is running on port 8080\n");

    while (1) {
        int client_socket = accept_client(server_socket);
        char *message = receive_message(client_socket);
        printf("Received: %s\n", message);
        send_message(client_socket, "Hello from server!");
        free(message);
        close_connection(client_socket);
    }

    close_connection(server_socket);
    return 0;
}
