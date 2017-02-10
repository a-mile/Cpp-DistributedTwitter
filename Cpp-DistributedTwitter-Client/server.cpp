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

server::server() {}

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

int server::connect() {
    if ((descriptor = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        return 0;
    }

    long arg;
    struct timeval tv;
    fd_set myset;
    socklen_t lon;
    int valopt;

    if( (arg = fcntl(descriptor, F_GETFL, NULL)) < 0) {
        return 0;
    }
    arg |= O_NONBLOCK;
    if( fcntl(descriptor, F_SETFL, arg) < 0) {
        return 0;
    }

    int res = ::connect(descriptor, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (res < 0) {
        if (errno == EINPROGRESS) {
            do {
                tv.tv_sec = 1;
                tv.tv_usec = 0;
                FD_ZERO(&myset);
                FD_SET(descriptor, &myset);
                res = select(descriptor+1, NULL, &myset, NULL, &tv);
                if (res < 0 && errno != EINTR) {
                    return 0;
                }
                else if (res > 0) {
                    lon = sizeof(int);
                    if (getsockopt(descriptor, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) {
                        return 0;
                    }
                    if (valopt) {
                        return 0;
                    }
                    break;
                }
                else {
                    return 0;
                }
            }while (1);
        }
        else {
            return 0;
        }
    }

    if( (arg = fcntl(descriptor, F_GETFL, NULL)) < 0) {
        return 0;
    }
    arg &= (~O_NONBLOCK);
    if( fcntl(descriptor, F_SETFL, arg) < 0) {
        return 0;
    }

    return 1;
}

int server::getDescriptor() {
    return descriptor;
}

void server::disconnect() {
    close(descriptor);
}

const char* server::getName() {
    return this->name;
}

const char* server::getIp() {
    return this->ip;
}

int server::getPort() {
    return this->port;
}

