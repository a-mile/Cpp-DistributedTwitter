#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <ctime>

using namespace std;

const int messageSize = 400;
const int postSize = 200;
const char* author = "Amadeusz";

void writePost(int server)
{
    char* message = new char[messageSize];
    char* post = new char[postSize];
    const char* delimiter = "&";

    cout<<"Napisz treść posta:"<<endl;
    cin.get();
    cin.getline(post, postSize);

    char* reqType = new char;
    *reqType |= 1<<0;

    char* date = new char[20];

    time_t now;
    struct tm *local;
    time (&now);
    local = localtime(&now);
    char* time = asctime(local);
    time[strlen(time)-1]=0;
    sprintf(date, "%s", time);

    strcpy(message,reqType);
    strcat(message,date);
    strcat(message,delimiter);
    strcat(message,author);
    strcat(message,delimiter);
    strcat(message, post);
    strcat(message,delimiter);

    write(server, message, messageSize);
}

void readPosts(int server)
{
    char* reqType = new char;

    write(server,reqType,1);

    char* message = new char[messageSize];
    while(read(server,message,messageSize) > 0) {
        write(1, message, messageSize);
        cout<<endl;
    }
}


int main(int argc, char **argv)
{
    int server;
    int port = 3000;
    string host = "localhost";

    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_aton (host.c_str(), &server_addr.sin_addr);
    server_addr.sin_port = htons(port);

    if ((server = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror ("Nie można utworzyć gniazdka");
        exit (EXIT_FAILURE);
    }

    if (connect (server, (struct sockaddr*) &server_addr, sizeof server_addr) < 0) {
        perror ("Brak połączenia");
        exit (EXIT_FAILURE);
    }

    int choose;
    cout<<"1 - Napisz nowy post"<<endl;
    cout<<"2 - Przeglądaj posty"<<endl;
    cin>>choose;

    switch(choose){
        case 1:
            writePost(server);
            break;
        case 2:
            readPosts(server);
            break;
        default:
            break;
    }

    close(server);
    return 0;
}