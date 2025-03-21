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

	chatingT.detach();
	reqT.join();

	//끝말잇기 함수가 끝나면 프로세스 종료
	closesocket(clientSock);
}

void Client::handleChating() {
	Packet packet;
	system("cls");
	while (1)
	{
		Clear_line(1, 23);
		gotoxy(1, 23);

		cout << "입력 : ";
		packet.type = 2;
		getline(cin, packet.buffer);
		cout << packet.buffer;
		send(clientSock, reinterpret_cast<const char*>(&packet.type), sizeof(packet.type), 0);
		int len = static_cast<int>(packet.buffer.size());
		send(clientSock, reinterpret_cast<const char*>(&len), sizeof(len), 0);
		if (len > 0)
			send(clientSock, packet.buffer.c_str(), len, 0);
	}
}

void Client::handleReq() {
	Packet packet;

	while (true)
	{
		getServerRes(packet);
		if (Player_life == 0)
			break;

		if (packet.type == 1)
		{
			if (packet.buffer == "0")
			{
				Clear_line(PRINT_WORD_CHAIN_X, 9);
				gotoxy(PRINT_WORD_CHAIN_X, 9);
				printf("단어가 없습니다");
				Clear_line(1, 23);
				gotoxy(1, 23);
			}
			else if (packet.buffer == "1")
			{
				Clear_line(PRINT_WORD_CHAIN_X, 9);
				gotoxy(PRINT_WORD_CHAIN_X, 9);
				printf("이미 사용된 단어입니다.");
				Clear_line(1, 23);
				gotoxy(1, 23);
			}
			else if (packet.buffer == "-2")
			{
				Clear_line(PRINT_WORD_CHAIN_X, 9);
				gotoxy(PRINT_WORD_CHAIN_X, 9);
				printf("전 단어와 현 단어의 앞, 뒤가 다릅니다.");
				Clear_line(1, 23);
				gotoxy(1, 23);
			}
			else if (packet.buffer == "error")
			{
				// 아무 처리를 안해주기위해 일부러 비워줬습니다.
			}
			else
			{
				Clear_line(PRINT_WORD_CHAIN_X, 9);
				gotoxy(PRINT_WORD_CHAIN_X, 9);
				printf("단어가 존재합니다.");
				Clear_line(PRINT_WORD_CHAIN_X, 14);
				gotoxy(PRINT_WORD_CHAIN_X, 14);
				printf("전 단어 : %s", packet.buffer);
				Clear_line(1, 23);
				gotoxy(1, 23);
			}
		}
		else if (packet.type == 2)
		{
			for (int k = 19; k >= 0; k--)
			{
				if (k == 0) // 맨 아래 채팅창의 배열에 입력된 문자열을 대입
					chatingChang[k] = packet.buffer;
				else // 아닐경우 채팅창 배열을 한칸씩 올림					
					chatingChang[k] = chatingChang[k - 1];

				//현재 위치의 화면 초기화 및 이동, 맨위 부터 출력
				Clear_line(1, 20 - k);
				gotoxy(1, 20 - k);
				cout << chatingChang[k];
			}
			gotoxy(1, 23);
		}
		else if (packet.type == 3)
		{
			if (packet.buffer == "0" || packet.buffer == "1" || packet.buffer == "-1")
				Player_life--;
		}

		Clear_line(PRINT_WORD_CHAIN_X, 5);
		gotoxy(PRINT_WORD_CHAIN_X, 5);
		cout << "Player Life : " << Player_life;
		Clear_line(1, 23);
		gotoxy(1, 23);
	}
}

void Client::getServerRes(Packet& packet) {
	int res = recv(clientSock, reinterpret_cast<char*>(&packet.type), sizeof(packet.type), 0);
	if (res == -1) return;
	if (res == 0) return;
	int len = 0;
	recv(clientSock, reinterpret_cast<char*>(&len), sizeof(len), 0);
	if (len > 0) {
		char* buffer = new char[len];
		int recived = recv(clientSock, buffer, len, 0);
		if (recived <= 0) {
			delete[] buffer;
			return;
		}
		packet.buffer.assign(buffer, recived);
	}
}