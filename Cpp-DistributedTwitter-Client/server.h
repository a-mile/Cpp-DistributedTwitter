#ifndef CPP_DISTRIBUTEDTWITTER_CLIENT_SERVER_H
#define CPP_DISTRIBUTEDTWITTER_CLIENT_SERVER_H

#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <ctime>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

class server {
private:
    const char* ip;
    const char* name;
    int port;
    int descriptor;
    struct sockaddr_in server_addr;
public:
    server(const char*,const char*, int);
    char* getInfo();
    int connect();
    int getDescriptor();
    void disconnect();
    const char* getName();
    const char* getIp();
    int getPort();
};

#endif
