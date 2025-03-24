#pragma once
#include <string>
#include <winsock2.h>
#include "type.h"
#include "chatWindow.h"

#define WIN32_LEAN_AND_MEAN
#define PRINT_WORD_CHAIN_X 50

using namespace std;

class Client {
private:
	string ip, port;
	SOCKET clientSock;
	WSADATA wsa;
	SOCKADDR_IN serverAddr;
	ChatWindow chatWindow;

	int Player_life = 10;
	void WSAInit();
	void socketInit(string port, string ip);
	void handleChating();
	void handleReq();
	void getServerRes(Packet& packet);
public:
	Client(string port, string ip = "127.0.0.1");
	~Client();
	void serverConnect();
};