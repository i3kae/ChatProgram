#include <iostream>
#include <winsock2.h>
#include <Windows.h>
#include <process.h>
#include <thread>
#include <mutex>
#include "server.h"
#include "constant.h"
#include "clientSession.h"

using namespace std;

// IP를 지정해주지 않을 경우 모든 IP(0.0.0.0)에서 허용
Server::Server(u_short port, vector<string> wordList, u_long ip)
: ip(ip), port(port), wordList(wordList), wordCount(wordList.size()) {
	this->WSAInit();
	this->serverSock = this->socketInit(port, ip);
}

Server::~Server() {
	closesocket(serverSock);
	WSACleanup();
}

void Server::WSAInit() {
	if (WSAStartup(MAKEWORD(2, 2), &(wsa)) != 0) // was 초기화
	{
		cout << "초기화 실패\n";
		return;
	}
}

SOCKET Server::socketInit(u_short port, u_long ip) {
	SOCKET sock;
	SOCKADDR_IN sockinfo;

	// SOCKET socket(af,type,protocol);
	// af = 주소체계 , type = 소켓타입 , protocol = 프로토콜
	// 인터넷 영역을 사용하는 TCP방식의 프로토콜
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		cout << "소켓 생성 실패\n";
		throw SERVER_ERROR::SOCKET_CREATE_ERROR;
	}

	memset(&sockinfo, 0x00, sizeof(sockinfo));
	sockinfo.sin_family = AF_INET; // sockinfo 구조체의 sin_family에 인터넷 주소 사용
	sockinfo.sin_port = htons(port); // 포트 지정
	sockinfo.sin_addr.s_addr = htonl(ip);


	// int bind (소켓 값,(SOCKADDR*)&소켓 주소 구조체 저장값, sizeof(소켓 구조체 저장 값))
	// bind를 사용하여 소켓 구조체의 정보를 서버 소켓에 저장
	if (bind(sock, (SOCKADDR*)&sockinfo, sizeof(sockinfo)) == SOCKET_ERROR)
	{
		cout << "bind 실패";
		throw SERVER_ERROR::SOCKET_BIND_ERROR;
	}

	return sock;
}

void Server::serverStart(int backlog) {
	// int listen(SOCKET, backlog)
	// SOCKET :소켓 생성시 만들어서 저장했던 소켓 핸들 값
	// backlog : 클라이언트의 연결 요청 대기 최대 개수
	if (listen(serverSock, backlog) == SOCKET_ERROR)
	{
		cout << "대기열 생성 실패\n";
		throw SERVER_ERROR::SOCKET_LISTEN_ERROR;
	}

	cout << "클라이언트로부터 접속을 기다리고 있습니다...\n";
	this->messageQueuing();
	this->listenClient();
}

void Server::messageQueuing() {
	thread t([this]() -> void {
		while (true) {
			if (!messageQueue.empty()) {
				// 필요할 때 추가로 사용
				SOCKET clientSock = messageQueue.front().first;
				Packet* packet = messageQueue.front().second;
				cout << clientSock << " broadCast " << packet->buffer << "\n";
				messageQueue.pop();
				this->broadcastPacket(packet);
				delete packet;
			}
		}
	});
	t.detach();
}

// 현재 clientSession에 연결되어 있는 모든 Client에 패킷 전송
void Server::broadcastPacket(Packet *packet) {
	for (ClientSession &client : clientSession)
		client.sendMsg(packet);
}

void Server::listenClient() {
	SOCKET clientSock;
	SOCKADDR_IN clientinfo;
	int clientSize = sizeof(clientinfo);

	while (1)
	{
		// accept(서버 소켓,((SOCKADDR*)&클라이언트 주소 값, &클라이언트 주소값 사이즈))
		// 클라이언트와 연결 될 소켓을 만들어 주어야 하므로 clientSock 변수 선언
		clientSock = accept(serverSock, (SOCKADDR*)&clientinfo, &clientSize);

		if (clientSock == INVALID_SOCKET)
		{
			cout << "클라이언트 소켓 연결 실패";
			throw SERVER_ERROR::SOCKET_CON_ERROR;
		}
		cout << "클라이언트 연결 성공 : " << clientSock << "\n";

		clientSession.emplace_back(clientSock, wordList, dictionaryList, messageQueue, mqMutex);
		thread t(&ClientSession::handleClientSession, std::ref(clientSession.back()));
		t.detach();
	}
}
