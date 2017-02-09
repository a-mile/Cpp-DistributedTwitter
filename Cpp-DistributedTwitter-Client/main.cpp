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
const int authorNameSize = 30;
const int postSize = 200;
const int postLimit = 20;

char* author = new char[authorNameSize];
int port;
const char* ip = "127.0.0.1";

server myServer(author,ip,port);
vector<server> servers;
vector<struct post> posts;

struct post{
    int seconds;
    string date;
    string author;
    string content;
};

bool comparePosts(const struct post &a, const struct post &b)
{
    return a.seconds < b.seconds;
}

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
    char* seconds = new char[20];

    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(date,20,"%d-%m-%Y %I:%M:%S",timeinfo);

    time_t sec = time(NULL);
    sprintf(seconds,"%ld",sec);

    strcpy(message,reqType);
    strcat(message,seconds);
    strcat(message,delimiter);
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

    posts.clear();

    for(int i = 0; i < servers.size(); i++) {
        if(servers[i].connect()) {

            write(servers[i].getDescriptor(), reqType, 1);

            char *message = new char[messageSize];
            while (read(servers[i].getDescriptor(), message, messageSize) > 0) {
                size_t pos = 0; 
                string token;  
                string messageString(message);

                pos = messageString.find(delimiter); 
                string seconds = messageString.substr(0, pos); 
                messageString.erase(0, pos + delimiter.length()); 
                pos = messageString.find(delimiter); 
                string date = messageString.substr(0, pos); 
                messageString.erase(0, pos + delimiter.length()); 
                pos = messageString.find(delimiter); 
                string author = messageString.substr(0, pos); 
                messageString.erase(0, pos + delimiter.length()); 
                pos = messageString.find(delimiter); 
                string content = messageString.substr(0, pos);

                struct post currentPost{
                        stoi(seconds),
                        date,
                        author,
                        content
                };

                posts.push_back(currentPost);
            }

            servers[i].disconnect();
        }
    }

    sort(posts.begin(), posts.end(), comparePosts);

    if(posts.size() == 0)
    {
        cout<<"Brak postów"<<endl;
    }
    else {
        for (int i = 0; (i < postLimit) && (i < posts.size()); i++) {
            cout << posts[i].date << endl;
            cout << posts[i].author << endl;
            cout << posts[i].content << endl;
        }
    }
}

void loadServers()
{
    servers.clear();
    servers.push_back(myServer);

    ifstream input("servers");

    while(true) {

        char* currentServerName = new char[authorNameSize];
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
    if(servers.size() == 1)
    {
        cout<<"Nie dodano żadnego serwera"<<endl;
    }
    else {
        for (int i = 1; i < servers.size(); i++) {
            cout << servers[i].getInfo() << endl;
        }
    }
}

void addServer()
{
    char* ip = new char[ipSize];
    char* name = new char[authorNameSize];
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

    loadServers();
}

void removeServer()
{
    showServerList();

    if(servers.size() > 1)
    {
        int number;

        cout<<"Podaj numer serwera, ktory chcesz usunac"<<endl;
        cin>>number;

        if(number >= 1 && number < servers.size())
        {
            servers.erase(servers.begin() + number);

            ofstream output("servers", fstream::out);
            for(int i=1; i< servers.size(); i++)
            {
                output << servers[i].getName();
                output << endl;
                output << servers[i].getIp();
                output << endl;
                output << servers[i].getPort();
                output << endl;
            }
            output.close();
        }

    }


}

void manageServers()
{
    int choose;
    cout<<"1 - Dodaj nowy serwer"<<endl;
    cout<<"2 - Usun serwer"<<endl;
    cout<<"3 - Pokaz liste serwerow"<<endl;

    cin>> choose;

    switch(choose){
        case 1:
            addServer();
            break;
        case 2:
            removeServer();
            break;
        case 3:
            showServerList();
            break;
        default:
            break;
    }
}

bool loadAuthor()
{
    ifstream input("author");

    if(!(input >> author)) {
        input.close();
        return false;
    }
    input >> port;
    input.close();

    return true;
}

int main(int argc, char **argv)
{
    loadServers();

    if(!loadAuthor())
    {
        cout<<"Nie skonfigurowano serwera"<<endl<<endl;
    }

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