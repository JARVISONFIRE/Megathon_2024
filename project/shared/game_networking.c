// game_networking.c
#include "game_networking.h"
#include <string.h>

static int server_socket;
static int client_sockets[MAX_CLIENTS];
static int num_clients = 0;
static float update_timer = 0.0f;

void init_game_networking(void) {
    server_socket = create_server(8080);
    printf("Game server started on port 8080\n");

    // Initialize client sockets array
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = -1;
    }
}

void handle_network_messages(void) {
    // Check for new connections
    int new_client = accept_client(server_socket);
    if (new_client >= 0 && num_clients < MAX_CLIENTS) {
        client_sockets[num_clients] = new_client;

        // Send join confirmation
        NetworkMessage msg = {
            .type = MSG_JOIN,
            .clientId = num_clients
        };
        send(new_client, &msg, sizeof(msg), 0);
        num_clients++;
    }

    // Handle messages from connected clients
    for (int i = 0; i < num_clients; i++) {
        if (client_sockets[i] >= 0) {
            NetworkMessage msg;
            int bytes_received = recv(client_sockets[i], &msg, sizeof(msg), MSG_DONTWAIT);

            if (bytes_received > 0) {
                // Process message based on type
                switch (msg.type) {
                    case MSG_PLACE_PUMPKIN:
                        // Broadcast pumpkin placement to all clients
                        for (int j = 0; j < num_clients; j++) {
                            if (j != i && client_sockets[j] >= 0) {
                                send(client_sockets[j], &msg, sizeof(msg), 0);
                            }
                        }
                        break;

                    case MSG_SHOOT:
                        // Broadcast shooting action to all clients
                        for (int j = 0; j < num_clients; j++) {
                            if (j != i && client_sockets[j] >= 0) {
                                send(client_sockets[j], &msg, sizeof(msg), 0);
                            }
                        }
                        break;
                }
            }
        }
    }
}

void broadcast_game_state(GameState *state) {
    NetworkMessage msg = {
        .type = MSG_SYNC,
        .gameState = *state
    };

    for (int i = 0; i < num_clients; i++) {
        if (client_sockets[i] >= 0) {
            send(client_sockets[i], &msg, sizeof(msg), 0);
        }
    }
}
