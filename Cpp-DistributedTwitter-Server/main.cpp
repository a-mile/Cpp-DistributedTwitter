#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fstream>

using namespace std;

const int messageSize = 400;

typedef enum requestType{
    get,
    post
};

enum requestType getRequestType(int sck)
{
    char req;
    read(sck, &req ,1);

    enum requestType reqType = static_cast<requestType>(req);

    return reqType;
}

void* client_loop(void *arg) {
    char* errorMessage = new char[messageSize];
    strcpy(errorMessage,"Nie udało się przetworzyć zapytania");

    int sck = *((int*) arg);

    enum requestType reqType  = getRequestType(sck);

    switch(reqType)
    {
        case requestType::get : {
            //Odczytaj wiadomości

            char *message = new char[messageSize];
            ifstream input("posts");
            string messageStr;

            while (getline(input, messageStr)) {
                strcpy(message, messageStr.c_str());
                write(sck, message, messageSize);
            }

            input.close();

            break;
        }
        case requestType::post : {
            //Zapisz wiadomość

            char *message = new char[messageSize];
            read(sck, message, messageSize);

            ofstream output("posts", fstream::out | fstream::app);
            output << message;
            output << endl;
            output.close();

            break;
        }
        default: {
            //Wyślij informację o błędzie

            write(sck, errorMessage, messageSize);
            break;
        }
    }

    close(sck);

    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    int port = 3000;
    struct sockaddr_in server_addr,client_addr;
    socklen_t clientlen = sizeof(client_addr);

    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int server = socket(PF_INET,SOCK_STREAM,0);
    if (!server) {
        perror("Nie udało się utworzyć gniazda");
        exit(-1);
    }

    int reuse = 1;
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("Nie udało się ustawić portu gniazda do ponownego użytku");
        exit(-1);
    }

    if (::bind(server,(const struct sockaddr *)&server_addr,sizeof(server_addr)) == -1) {
        perror("Nie można powiązać gniazda z adresem lokalnej maszyny");
        exit(-1);
    }

    if (listen(server,SOMAXCONN) == -1) {
        perror("Nie można ustawić rozmiaru kolejki gniazda");
        exit(-1);
    }

    int client;

    while (1) {
        client = accept(server, (struct sockaddr*)&client_addr, &clientlen);

        if (client < 0)
        {
            perror("Nie można ustworzyć połączenia z gniazdem");
            exit(-1);
        }

        pthread_t id;

        pthread_create(&id, NULL, client_loop, &client);
    }

    close(server);
    return 0;
}