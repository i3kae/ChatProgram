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
	char word[30], sameword[30], prev_word[31];
	Packet* packet = (Packet*)malloc(sizeof(Packet));

	while (recv(this->clientSock, (char*)packet, sizeof(Packet), 0) != -1) {
		// recv �Լ�: ��ȯ���� ���� ������ ũ�� , �����ϸ� -1
		// ���� ���� �ܾ�� packet�� ����
		cout << "Type: " << packet->type << "\n";
		cout << "Buffer: " << packet->buffer << "\n";

		if (packet->type == 1) {
			// �ܾ ��ġ�ϴ� �˻� -> �ܾ��忡 �ִ��� <����1>
			// �ܾ� ���ڸ��� �� ���� �Դ� ���ڸ��� ������ <����2>
			// ������?
			//		�� �ܾ ��ȯ
			// �ٸ���?
			//		0
			// �ߺ��Ǹ�?
			//		1
			//1.���ϱ��̱��ϱ�
			isIn = 0;
			for (string word : dictionaryList) {
				if (word == packet->buffer) {
					// ������ �־����� ? -> 1
					// ������ -> �ܾ��ȯ	
					isIn = 1;
					break;
				}
			}

			if (isIn) {
				//word �˻��ؼ� ������ 1
				// ������ �ܾ�
				isIn = 0;

				for (string word : wordList) {
					if (word == packet->buffer) {
						isIn = 1;
						break;
					}
				}
				if (isIn) {
					strcpy(packet->buffer, "1");
				}
				else {
					wordList.emplace_back(packet->buffer);
				}
			}
			else
			{
				strcpy(packet->buffer, "0");
			}

			// ������ �ܾ��忡 ���� �ܾ�� -> 0
			//��, �� ���ڰ� ������ üũ ù ��°�� ������ �Ѱ���

			if ((strcmp(packet->buffer, "0")) && (strcmp(packet->buffer, "1")))
			{
				if (flag != 0)
				{
					if (prev_word[strlen(prev_word) - 1] != packet->buffer[0])
						strcpy(packet->buffer, "-1");
				}
				strcpy(prev_word, packet->buffer);
			}
			if ((strcmp(packet->buffer, "0")) && (strcmp(packet->buffer, "1")) && (flag == 0))
				flag = 1;
			packet->type = 3;
			send(clientSock, (char*)packet, sizeof(Packet), 0);
			packet->type = 1;
			Sleep(10);
			PushMsgQueue(packet);
		}
		else if (packet->type == 2) {
			PushMsgQueue(packet);
		}
		else {
			strcpy(packet->buffer, "error");
			PushMsgQueue(packet);
		}
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