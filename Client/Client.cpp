#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <string>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <Windows.h>
#include <string.h>
#include "type.h"
#include "utils.h"
#include "client.h"

#define WIN32_LEAN_AND_MEAN
#define PRINT_WORD_CHAIN_X 50

using namespace std;

// DWORD는 더블워드 == long
// WINAPI 윈도우에서의 API
// LPVOID ms에서 사용하는 void포인터

Client::Client(string port, string ip)
	: port(port), ip(ip) {
	WSAInit();
	socketInit(port, ip);
}

Client::~Client() {
	closesocket(clientSock);
	WSACleanup();
}

// WSA 초기화
void Client::WSAInit() {
	if (WSAStartup(MAKEWORD(2, 2), &(wsa)) != 0)
	{
		cout << "WSA 초기화 실패\n";
		return;
	}
}

void Client::socketInit(string port, string ip) {
	clientSock = socket(PF_INET, SOCK_STREAM, 0);
	if (clientSock == INVALID_SOCKET) {
		cout << "소켓 생성 실패";
		return;
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
	serverAddr.sin_port = htons(atoi(port.c_str()));
}

void Client::serverConnect() {
	if (connect(clientSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cout << "connect() error!";
	}

	//쓰레드를 생성
	thread chatingT(&Client::handleChating, this);
	thread reqT(&Client::handleReq, this);

	// 응답 스레드가 종료 될 때까지 대기
	chatingT.detach();
	reqT.join();
}

void Client::handleChating() {
	Packet packet;
	system("cls");
	while (1)
	{
		Clear_line(1, 23);
		gotoxy(1, 23);

		cout << "입력 : ";
		getline(cin, packet.buffer);
		packet.len = static_cast<int>(packet.buffer.size());
		send(clientSock, reinterpret_cast<const char*>(&packet.len), sizeof(packet.len), 0);
		if (packet.len > 0)
			send(clientSock, packet.buffer.c_str(), packet.len, 0);
	}
}

void Client::handleReq() {
	Packet packet;

	while (true) {
		getServerRes(packet);
		chatWindow.updateChat(packet.buffer);
		chatWindow.printChat();
	}
}

void Client::getServerRes(Packet& packet) {
	int res = recv(clientSock, reinterpret_cast<char*>(&packet.len), sizeof(packet.len), 0);
	if (res <= 0) return;
	if (packet.len > 0) {
		char* buffer = new char[packet.len];
		int recived = recv(clientSock, buffer, packet.len, 0);
		if (recived <= 0) {
			delete[] buffer;
			return;
		}
		packet.buffer.assign(buffer, recived);
	}
}