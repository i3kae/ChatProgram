#include <winsock2.h>
#include <vector>
#include <mutex>
#include <string>
#include <iostream>
#include <queue>
#include "clientSession.h"
#include "constant.h"

using namespace std;

ClientSession::ClientSession(SOCKET clientSock, vector<string>& wordList, vector<string>& dictionaryList, queue<pair<SOCKET, Packet *>>& messageQueue, mutex& mqMutex)
	: clientSock(clientSock), wordList(wordList), dictionaryList(dictionaryList), messageQueue(messageQueue), mqMutex(mqMutex) {
}

ClientSession::~ClientSession() {
	cout << "연결 종료 : " << clientSock << "\n";
	closesocket(clientSock);
}

void ClientSession::handleClientSession() {
	// recv 함수: 반환값이 받은 데이터 크기 , 실패하면 -1
	try {
		while (true) {
			Packet* packet = new Packet;
			getServerRes(packet);
			cout << "Type: " << packet->type << "\n";
			cout << "Buffer: " << packet->buffer << "\n";
			packet->type = 2;
			pushMQ(packet);
		}
	}
	catch (const CLIENT_SESSION_ERROR &e) {}
}

// 전달 된 메시지를 메시지 큐에 전달
void ClientSession::pushMQ(Packet* packet) {
	lock_guard<mutex> lock(mqMutex);
	messageQueue.push({ clientSock, packet });
}

// 자신의 클라이언트에 메시지 전달
void ClientSession::sendMsg(Packet* packet) {
	int len = packet->buffer.length();
	send(clientSock, reinterpret_cast<char*>(&packet->type), sizeof(packet->type), 0);
	send(clientSock, reinterpret_cast<char*>(&len), sizeof(len), 0);
	if (len > 0)
		send(clientSock, packet->buffer.c_str(), len, 0);
}

void ClientSession::getServerRes(Packet* packet) {
	int len = 0;
	int res = recv(clientSock, reinterpret_cast<char*>(&packet->type), sizeof(packet->type), 0);
	if (res < 0)
		throw CLIENT_SESSION_ERROR::CLIENT_RECV_ERROR;
	recv(clientSock, reinterpret_cast<char*>(&len), sizeof(len), 0);
	if (len > 0) {
		char* buffer = new char[len];
		int recived = recv(clientSock, buffer, len, 0);
		if (recived <= 0) {
			delete[] buffer;
			return;
		}
		packet->buffer.assign(buffer, recived);
	}
}