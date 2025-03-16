#include <winsock2.h>
#include <vector>
#include <mutex>
#include <string>
#include <iostream>
#include <queue>
#include "ClientSession.h"

using namespace std;

ClientSession::ClientSession(SOCKET clientSock, vector<string>& wordList, vector<string>& dictionaryList, queue<pair<SOCKET, Packet *>>& messageQueue, mutex& mqMutex)
	: clientSock(clientSock), wordList(wordList), dictionaryList(dictionaryList), messageQueue(messageQueue), mqMutex(mqMutex) {
}

unsigned WINAPI ClientSession::HandleClientRes() {
	int strLen = 0, isIn = 0;
	string word, sameword, prevWord;
	Packet* packet = (Packet*)malloc(sizeof(Packet));

	// recv 함수: 반환값이 받은 데이터 크기 , 실패하면 -1
	while (recv(this->clientSock, (char*)packet, sizeof(Packet), 0) != -1) {
		cout << "Type: " << packet->type << "\n";
		cout << "Buffer: " << packet->buffer << "\n";
		packet->type = 2;
		PushMsgQueue(packet);
	}

	// 소켓 연결 종료
	closesocket(clientSock);
	return 0;
}

// 전달 된 메시지를 메시지 큐에 전달한다.
void ClientSession::PushMsgQueue(Packet* packet) {
	mqMutex.lock();
	messageQueue.push({ clientSock, packet });
	mqMutex.unlock();
}

// 자신의 클라이언트에 메시지 전달
void ClientSession::SendMsg(Packet* packet) {
	 send(clientSock, (char*)packet, sizeof(Packet), 0); 
}