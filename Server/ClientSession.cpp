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
	// recv �Լ�: ��ȯ���� ���� ������ ũ�� , �����ϸ� -1
	while (true) {
		Packet* packet = new Packet;
		int res = recv(this->clientSock, reinterpret_cast<char*>(&packet->type), sizeof(packet->type), 0);
		if (res == -1) break;
		if (res == 0) continue;
		int len = 0;
		recv(this->clientSock, reinterpret_cast<char*>(&len), sizeof(len), 0);
		if (len > 0) {
			char* buffer = new char[len];
			int recived = recv(this->clientSock, buffer, len, 0);
			if (recived <= 0) {
				delete[] buffer;
				break;
			}
			packet->buffer.assign(buffer, recived);
		}
		cout << res << "\n";
		cout << "Type: " << packet->type << "\n";
		cout << "Buffer: " << packet->buffer << "\n";
		
		packet->type = 2;
		PushMsgQueue(packet);
	}
	// ���� ���� ����
	cout << "���� ���� : " << clientSock << "\n";
	closesocket(clientSock);
	return 0;
}

// ���� �� �޽����� �޽��� ť�� ����
void ClientSession::PushMsgQueue(Packet* packet) {
	mqMutex.lock();
	messageQueue.push({ clientSock, packet });
	mqMutex.unlock();
}

// �ڽ��� Ŭ���̾�Ʈ�� �޽��� ����
void ClientSession::SendMsg(Packet* packet) {
	int len = packet->buffer.length();
	send(clientSock, reinterpret_cast<char*>(&packet->type), sizeof(packet->type), 0);
	send(clientSock, reinterpret_cast<char*>(&len), sizeof(len), 0);
	if (len > 0)
		send(clientSock, packet->buffer.c_str(), len, 0);
}