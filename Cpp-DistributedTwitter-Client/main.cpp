#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <ctime>
#include <vector>

#include "server.h"

using namespace std;

const int messageSize = 400;
const int ipSize = 20;
const int serverNameSize = 20;
const int postSize = 200;

const char* author = "Amadeusz";
const char* ip = "127.0.0.1";
const int port = 3000;

server myServer(author,ip,port);
vector<server> servers;

void writePost()
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

    if(myServer.connect()) {
        write(myServer.getDescriptor(), message, messageSize);
        myServer.disconnect();
    }
}

void readPosts()
{
    char* reqType = new char;
    string delimiter = "&";

    for(int i = 0; i < servers.size(); i++) {
        if(servers[i].connect()) {

            write(servers[i].getDescriptor(), reqType, 1);

            char *message = new char[messageSize];
            while (read(servers[i].getDescriptor(), message, messageSize) > 0) {
                size_t pos = 0; 
                string token;  
                string messageString(message);

                pos = messageString.find(delimiter); 
                string date = messageString.substr(0, pos); 
                messageString.erase(0, pos + delimiter.length()); 
                pos = messageString.find(delimiter); 
                string author = messageString.substr(0, pos); 
                messageString.erase(0, pos + delimiter.length()); 
                pos = messageString.find(delimiter); 
                string content = messageString.substr(0, pos);

                cout << date << endl;
                cout << author << endl;
                cout << content << endl;
            }

            servers[i].disconnect();
        }
    }
}

void loadServers()
{
    servers.push_back(myServer);

    ifstream input("servers");

    while(true) {

        char* currentServerName = new char[serverNameSize];
        char* currentServerIp = new char[ipSize];
        int currentServerPort;

        if(!(input >> currentServerName))
            break;
        input >> currentServerIp;
        input >> currentServerPort;

        server currentServer(currentServerName, currentServerIp, currentServerPort);

        servers.push_back(currentServer);
    }

    input.close();
}

void showServerList()
{
    for(int i = 0; i < servers.size(); i++)
    {
        cout<<servers[i].getInfo()<<endl;
    }
}

void addServer()
{
    char* ip = new char[ipSize];
    char* name = new char[serverNameSize];
    int port;

    cout<<"Podaj nazwe serwera"<<endl;
    cin>>name;
    cout<<"Podaj adres IP serwera"<<endl;
    cin>>ip;
    cout<<"Podaj numer portu serwera"<<endl;
    cin>>port;

    ofstream output("servers", fstream::out | fstream::app);

    output << name;
    output << endl;
    output << ip;
    output << endl;
    output << port;
    output << endl;
    output.close();
}

void manageServers()
{
    int choose;
    cout<<"1 - Dodaj nowy serwer"<<endl;
    cout<<"2 - Pokaz liste serwerow"<<endl;
    cin>> choose;

    switch(choose){
        case 1:
            addServer();
            break;
        case 2:
            showServerList();
            break;
        default:
            break;
    }
}

int main(int argc, char **argv)
{
    loadServers();

    while(true) {
        int choose;

        cout << "1 - Napisz nowy post" << endl;
        cout << "2 - Przeglądaj posty" << endl;
        cout << "3 - Zarzadzaj serwerami" << endl;
        cout << "4 - Zamknij" << endl;
        cin >> choose;

        if(choose == 4)
            break;

        switch (choose) {
            case 1:
                writePost();
                break;
            case 2:
                readPosts();
                break;
            case 3:
                manageServers();
            default:
                break;
        }
    }
    return 0;
}