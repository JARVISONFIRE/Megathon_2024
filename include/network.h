// network.h
#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>  // Include this to define size_t
#include <sys/socket.h>  // Include for socket functions
#include <netinet/in.h>  // Include for sockaddr_in structure
#include <arpa/inet.h>  // Include for inet_pton function

int create_server_socket(int port);
int accept_client(int server_socket);
int create_client_socket(const char *ip_address, int port);
int send_data(int socket, const char *data, size_t length);
int receive_data(int socket, char *buffer, size_t length);
void close_socket(int socket);

#endif // NETWORK_H
