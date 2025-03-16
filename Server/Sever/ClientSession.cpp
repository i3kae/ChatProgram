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
		// recv 함수: 반환값이 받은 데이터 크기 , 실패하면 -1
		// 지금 받은 단어는 packet에 있음
		cout << "Type: " << packet->type << "\n";
		cout << "Buffer: " << packet->buffer << "\n";

		if (packet->type == 1) {
			// 단어가 일치하는 검사 -> 단어장에 있는지 <파일1>
			// 단어 앞자리가 그 전에 왔던 뒷자리랑 같은지 <파일2>
			// 같으면?
			//		그 단어를 반환
			// 다르면?
			//		0
			// 중복되면?
			//		1
			//1.파일길이구하기
			isIn = 0;
			for (string word : dictionaryList) {
				if (word == packet->buffer) {
					// 기존에 있었던거 ? -> 1
					// 없던거 -> 단어반환	
					isIn = 1;
					break;
				}
			}

			if (isIn) {
				//word 검사해서 있으면 1
				// 없으면 단어
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

			// 위에서 단어장에 없는 단어면 -> 0
			//앞, 뒤 글자가 같은지 체크 첫 번째는 무조건 넘겨줌

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