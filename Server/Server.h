#pragma once
#include <vector>
#include <list>
#include <queue>
#include <string>
#include <mutex>
#include "type.h"
#include "clientSession.h"

using namespace std;

class Server{
private:
    SOCKET serverSock;
    list<ClientSession> clientSession;
    WSADATA wsa;
    u_long ip;
    u_short port;
    mutex mqMutex;
    vector<string> dictionaryList;
    vector<string> wordList;
    queue<pair<SOCKET, Packet *>> messageQueue;

    int dictionaryCount = 0;
    int wordCount = 0;

    void WSAInit();
    SOCKET socketInit(u_short port, u_long ip);
    void broadcastPacket(Packet *packet);
    void messageQueuing();
    void listenClient();
public:
    Server(u_short port, vector<string> wordList, u_long ip = INADDR_ANY);
    ~Server();
    void serverStart(int backlog);
    void removeClientSession(ClientSession& session);
};