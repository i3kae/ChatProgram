#pragma once
#include <winsock2.h>
#include <mutex>
#include <queue>
#include "type.h"

using namespace std;

class ClientSession {
public:
	SOCKET clientSock;
	vector<string> &dictionaryList;
	vector<string> &wordList;
	queue<pair<SOCKET, Packet *>> &messageQueue;
	recursive_mutex& mqMutex;
	int flag = 0;

	ClientSession(SOCKET clientSock, vector<string> &wordList, vector<string> &dictionaryList, queue<pair<SOCKET, Packet *>> &messageQueue, recursive_mutex&mqMutex);
	void pushMQ(Packet* packet);
	unsigned WINAPI handleClientSession();
	void sendMsg(Packet* packet);
	void getServerRes(Packet* packet);
};