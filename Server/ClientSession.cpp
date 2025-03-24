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
	cout << "���� ���� : " << clientSock << "\n";
	closesocket(clientSock);
}

void ClientSession::handleClientSession() {
	// recv �Լ�: ��ȯ���� ���� ������ ũ�� , �����ϸ� -1
	try {
		while (true) {
			Packet* packet = new Packet;
			getServerRes(packet);
			cout << "Type: " << packet->len << "\n";
			cout << "Buffer: " << packet->buffer << "\n";
			pushMQ(packet);
		}
	}
	catch (const CLIENT_SESSION_ERROR &e) {}
}

// ���� �� �޽����� �޽��� ť�� ����
void ClientSession::pushMQ(Packet* packet) {
	lock_guard<mutex> lock(mqMutex);
	messageQueue.push({ clientSock, packet });
}

// �ڽ��� Ŭ���̾�Ʈ�� �޽��� ����
void ClientSession::sendMsg(Packet* packet) {
	send(clientSock, reinterpret_cast<char*>(&packet->len), sizeof(packet->len), 0);
	if (packet->len > 0)
		send(clientSock, packet->buffer.c_str(), packet->len, 0);
}

void ClientSession::getServerRes(Packet* packet) {
	int res = recv(clientSock, reinterpret_cast<char*>(&packet->len), sizeof(packet->len), 0);
	if (res < 0)
		throw CLIENT_SESSION_ERROR::CLIENT_RECV_ERROR;
	if (packet->len > 0) {
		char* buffer = new char[packet->len];
		int recived = recv(clientSock, buffer, packet->len, 0);
		if (recived <= 0) {
			delete[] buffer;
			return;
		}
		packet->buffer.assign(buffer, recived);
	}
}