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
bool configured;

server *myServer;
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
    if(!configured)
    {
        cout<<"Nie skonfigurowano serwera"<<endl;
        return;
    }

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

    //Data względna w sekundach, aby uprościć sortowanie
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

    if(myServer->connect()) {
        write(myServer->getDescriptor(), message, messageSize);
        myServer->disconnect();
    }
}

void readServerPosts(server* serv)
{
    char* reqType = new char;
    string delimiter = "&";

    if(serv->connect()) {

        write(serv->getDescriptor(), reqType, 1);

        char *message = new char[messageSize];
        while (read(serv->getDescriptor(), message, messageSize) > 0) {
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

        serv->disconnect();
    } else{
        cout << "Nie można połączyć z "<<serv->getName()<<endl;
    }
}

void readAllPosts()
{
    posts.clear();

    if(configured)
        readServerPosts(myServer);

    for(int i=0; i < servers.size(); i++) {
        readServerPosts(&servers[i]);
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

bool showServerList()
{
    if(servers.size() == 0)
    {
        cout<<"Nie dodano żadnego serwera"<<endl;
        return false;
    }
    else {
        for (int i=0; i < servers.size(); i++) {
            cout <<i+1<<". "<< servers[i].getInfo() << endl;
        }
        return true;
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
    if(showServerList())
    {
        int number;

        cout<<"Podaj numer serwera, ktory chcesz usunac"<<endl;
        cin>>number;

        number++;

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
        } else{
            cout<<"Nie ma takiego serwera"<<endl;
        }

    }
}

void manageServers()
{
    while(true) {
        int choose;
        cout << "1 - Dodaj nowy serwer" << endl;
        cout << "2 - Usun serwer" << endl;
        cout << "3 - Pokaz liste serwerow" << endl;
        cout << "4 - Cofnij" << endl;
        cin >> choose;

        if (choose == 4)
            return;

        if (cin.fail()) {
            cin.clear();
            cin.ignore();
            cout << "Nie ma takiej opcji" << endl<<endl;
        } else {
            switch (choose) {
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
                    cout << "Nie ma takiej opcji" << endl<<endl;
            }
        }
    }
}

//Wczytaj nazwę i port własnego serwera
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
        configured = false;
        cout<<"Nie skonfigurowano serwera"<<endl;
    } else{
        configured = true;
        myServer = new server(author,ip,port);
    }

    while(true) {
        int choose;

        cout << "1 - Napisz nowy post" << endl;
        cout << "2 - Przeglądaj posty" << endl;
        cout << "3 - Zarzadzaj serwerami" << endl;
        cout << "4 - Zamknij" << endl;
        cin >> choose;

        if(cin.fail()){
            cin.clear();
            cin.ignore();
            cout << "Nie ma takiej opcji" << endl;
        } else {
            if (choose == 4)
                break;

            switch (choose) {
                case 1:
                    writePost();
                    break;
                case 2:
                    readAllPosts();
                    break;
                case 3:
                    manageServers();
                    break;
                default:
                    cout << "Nie ma takiej opcji" << endl;
            }
        }
    }
    return 0;
}