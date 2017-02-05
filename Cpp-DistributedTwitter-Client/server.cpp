#include "server.h"

server::server(const char * name, const char * ip, int port) {
    this->name = name;
    this->ip = ip;
    this->port = port;

    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_aton (ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);
}

char* server::getInfo() {
    char* info = new char[100];
    strcpy(info, this->name);
    strcat(info, " ");
    strcat(info, this->ip);
    strcat(info, ":");
    std::string port = std::to_string(this->port);
    strcat(info, port.c_str());

    return info;
}

void server::connect() {
    if ((descriptor = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror ("Nie można utworzyć gniazdka");
        exit (EXIT_FAILURE);
    }
    if (::connect (descriptor, (struct sockaddr*) &server_addr, sizeof server_addr) < 0) {
        perror ("Brak połączenia");
        exit (EXIT_FAILURE);
    }
}

int server::getDescriptor() {
    return descriptor;
}

void server::disconnect() {
    close(descriptor);
}

