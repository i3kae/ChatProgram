
#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<Windows.h>
#include<string.h>
#define WIN32_LEAN_AND_MEAN
#define PRINT_WORD_CHAIN_X 50
DWORD WINAPI ThreadFunc(LPVOID);
// DWORD는 더블워드 == long
// WINAPI 윈도우에서의 API
//LPVOID ms에서 사용하는 void포인터

typedef struct _packet {
	int type; // 1이면 단어입력, 2이면 채팅
	char buffer[101];
	char re[20];
	char name[21];
}Packet;

void ErrorHandling(char* message);
void gotoxy(int x, int y);
void Clear_line(x, y);

int Chating_line = 0;
char Chat[101];

// 출력될 채팅창 초기화
char Chatingchang[20][32] =
{
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  ",
	"  "
};

int main(int argc, char* argv[]) {
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;
	int Player_life = 3, strLen, k;
	char name[21];
	Packet packet;

	int strLen;
	if (argc != 3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ErrorHandling("WSAStartup() error!");
	}

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET) {
		ErrorHandling("socket() error");
	}
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons(atoi(argv[2]));

	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
		ErrorHandling("connect() error!");
	}

	printf("당신의 이름을 입력해 주세요 : ");
	gets_s(packet.buffer, 20);
	strcpy(name, packet.buffer);
	send(hSocket, (char*)&packet, sizeof(Packet), 0);

	while (Player_life != 0)
	{

		if (!kbhit())
		{
			printf("%s : ", name);
			gets_s(packet.buffer, 30);
			sned(hSocket, (char*)&packet, sizeof(Packet), 0);
		}
		else
		{
			if ((strLen = recv(hSocket, (char*)&packet, sizeof(packet), 0)) != -1)
				printf("ERROR!");
			else
			{
				if (packet.type == 1)
				{
					
				}
				else if (packet.type == 2)
				{
					for (k = 19; k >= 0; k--)
					{
						if (k == 0) // 맨 아래 채팅창의 배열에 입력된 문자열을 대입
						{
							strcpy(Chatingchang[k], packet.name);
							strcat(Chatingchang[k], " : ");
							strcpy(Chatingchang[k], packet.buffer);
						}
						else // 아닐경우 채팅창 배열을 한칸씩 올림
							strcpy(Chatingchang[k], Chatingchang[k - 1]);

						//현재 위치의 화면 초기화 및 이동, 맨위 부터 출력
						Clear_line(1, 20 - k);
						gotoxy(1, 20 - k);

						printf("%s", Chatingchang[k]);
					}
				}
			}
		}

	}

	closesocket(hSocket);
	WSACleanup();
	return 0;
}
void ErrorHandling(char* message) {
	fputs(message, stderr);
	fputc("\n", stderr);
	exit(1);
}

//x,y위치 화면 초기화
void Clear_line(x, y)
{
	gotoxy(x, y);
	printf("                                                            ");
}

//커서 이동 함수
void gotoxy(int x, int y)
{
	COORD pos = { x - 1, y - 1 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}