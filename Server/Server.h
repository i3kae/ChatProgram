#pragma once
#include <vector>
#include <queue>
#include <string>
#include <mutex>
#include "type.h"
#include "clientSession.h"

using namespace std;

class Server{
private:
    SOCKET serverSock;
    vector<ClientSession> clientSession;
    WSADATA wsa;
    u_long ip;
    u_short port;
    vector<string> dictionaryList;
    vector<string> wordList;
    queue<pair<SOCKET, Packet *>> messageQueue;
    shared_ptr<mutex> mqMutex;

    int dictionaryCount = 0;
    int wordCount = 0;

    void broadcastPacket(Packet *packet);
    void messageQueuing();
    void listenClient();
public:
    Server(u_short port, vector<string> wordList, u_long ip = INADDR_ANY);
    ~Server();
    void WSAInit();
    SOCKET socketInit(u_short port, u_long ip);
    void serverStart(int backlog);
};