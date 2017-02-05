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
    void connect();
    int getDescriptor();
    void disconnect();
};

#endif
