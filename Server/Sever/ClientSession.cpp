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

	// recv �Լ�: ��ȯ���� ���� ������ ũ�� , �����ϸ� -1
	while (recv(this->clientSock, (char*)packet, sizeof(Packet), 0) != -1) {
		cout << "Type: " << packet->type << "\n";
		cout << "Buffer: " << packet->buffer << "\n";
		packet->type = 2;
		PushMsgQueue(packet);
	}

	// ���� ���� ����
	closesocket(clientSock);
	return 0;
}

// ���� �� �޽����� �޽��� ť�� �����Ѵ�.
void ClientSession::PushMsgQueue(Packet* packet) {
	mqMutex.lock();
	messageQueue.push({ clientSock, packet });
	mqMutex.unlock();
}

// �ڽ��� Ŭ���̾�Ʈ�� �޽��� ����
void ClientSession::SendMsg(Packet* packet) {
	 send(clientSock, (char*)packet, sizeof(Packet), 0); 
}