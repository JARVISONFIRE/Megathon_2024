// networking.h
#ifndef NETWORKING_H
#define NETWORKING_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CONNECTIONS 10
#define BUFFER_SIZE 1024

int create_server(int port);
int accept_client(int server_socket);
void send_message(int client_socket, const char *message);
char *receive_message(int client_socket);
void close_connection(int socket);

#endif // NETWORKING_H
