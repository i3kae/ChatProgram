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

// IP�� ���������� ���� ��� ��� IP(0.0.0.0)���� ���
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
	if (WSAStartup(MAKEWORD(2, 2), &(wsa)) != 0) // was �ʱ�ȭ
	{
		cout << "�ʱ�ȭ ����\n";
		return;
	}
}

SOCKET Server::socketInit(u_short port, u_long ip) {
	SOCKET sock;
	SOCKADDR_IN sockinfo;

	// SOCKET socket(af,type,protocol);
	// af = �ּ�ü�� , type = ����Ÿ�� , protocol = ��������
	// ���ͳ� ������ ����ϴ� TCP����� ��������
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		cout << "���� ���� ����\n";
		throw SERVER_ERROR::SOCKET_CREATE_ERROR;
	}

	memset(&sockinfo, 0x00, sizeof(sockinfo));
	sockinfo.sin_family = AF_INET; // sockinfo ����ü�� sin_family�� ���ͳ� �ּ� ���
	sockinfo.sin_port = htons(port); // ��Ʈ ����
	sockinfo.sin_addr.s_addr = htonl(ip);


	// int bind (���� ��,(SOCKADDR*)&���� �ּ� ����ü ���尪, sizeof(���� ����ü ���� ��))
	// bind�� ����Ͽ� ���� ����ü�� ������ ���� ���Ͽ� ����
	if (bind(sock, (SOCKADDR*)&sockinfo, sizeof(sockinfo)) == SOCKET_ERROR)
	{
		cout << "bind ����";
		throw SERVER_ERROR::SOCKET_BIND_ERROR;
	}

	return sock;
}

void Server::serverStart(int backlog) {
	// int listen(SOCKET, backlog)
	// SOCKET :���� ������ ���� �����ߴ� ���� �ڵ� ��
	// backlog : Ŭ���̾�Ʈ�� ���� ��û ��� �ִ� ����
	if (listen(serverSock, backlog) == SOCKET_ERROR)
	{
		cout << "��⿭ ���� ����\n";
		throw SERVER_ERROR::SOCKET_LISTEN_ERROR;
	}

	cout << "Ŭ���̾�Ʈ�κ��� ������ ��ٸ��� �ֽ��ϴ�...\n";
	this->messageQueuing();
	this->listenClient();
}

void Server::messageQueuing() {
	thread t([this]() -> void {
		while (true) {
			if (!messageQueue.empty()) {
				// �ʿ��� �� �߰��� ���
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

// ���� clientSession�� ����Ǿ� �ִ� ��� Client�� ��Ŷ ����
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
		// accept(���� ����,((SOCKADDR*)&Ŭ���̾�Ʈ �ּ� ��, &Ŭ���̾�Ʈ �ּҰ� ������))
		// Ŭ���̾�Ʈ�� ���� �� ������ ����� �־�� �ϹǷ� clientSock ���� ����
		clientSock = accept(serverSock, (SOCKADDR*)&clientinfo, &clientSize);

		if (clientSock == INVALID_SOCKET)
		{
			cout << "Ŭ���̾�Ʈ ���� ���� ����";
			throw SERVER_ERROR::SOCKET_CON_ERROR;
		}
		cout << "Ŭ���̾�Ʈ ���� ���� : " << clientSock << "\n";

		clientSession.emplace_back(clientSock, wordList, dictionaryList, messageQueue, mqMutex);
		thread t(&ClientSession::handleClientSession, std::ref(clientSession.back()));
		t.detach();
	}
}
