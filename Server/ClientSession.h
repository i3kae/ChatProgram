#pragma once
#include <winsock2.h>
#include <mutex>
#include <queue>
#include "Type.h"

using namespace std;

class ClientSession {
public:
	SOCKET clientSock;
	vector<string> &dictionaryList;
	vector<string> &wordList;
	queue<pair<SOCKET, Packet *>> &messageQueue;
	mutex& mqMutex;
	int flag = 0;

	ClientSession(SOCKET clientSock, vector<string> &wordList, vector<string> &dictionaryList, queue<pair<SOCKET, Packet *>> &messageQueue, mutex &mqMutex);
	void PushMsgQueue(Packet* packet);
	unsigned WINAPI HandleClientRes();
	void SendMsg(Packet* packet);
};