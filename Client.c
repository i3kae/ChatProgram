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
}Packet;

void ErrorHandling(char* message);
void gotoxy(int x, int y);
void Clear_line(x, y);
void Print_Word(LPVOID n);
void Chating(LPVOID n);

int Chating_line = 0;
char Chat[101];
HANDLE hThrd_Word, hThrd_Chating;
DWORD threadId_Word, threadId_Chating;
WSADATA wsaData;
SOCKET hSocket;
SOCKADDR_IN servAddr;
Packet packet;

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


int main() {
	int strLen;
	int argc;
	char argv[10][1000];
	argc = 3;
	strcpy(argv[1], "127.0.0.1");
	strcpy(argv[2], "5959");
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

	//쓰레드를 생성
	hThrd_Word = CreateThread(NULL, 0, (DWORD WINAPI)Print_Word, 0, 0, &threadId_Word);
	hThrd_Chating = CreateThread(NULL, 0, (DWORD WINAPI)Chating, 0, 0, &threadId_Chating);

	//끝말잇기 함수가 끝나면 프로세스 종료
	WaitForSingleObject(hThrd_Word, INFINITE);

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

void Print_Word(LPVOID n)
{
	int k, Player_life = 3,strLen;

	while (recv(hSocket, (char*)&packet, sizeof(Packet), 0) != -1)
	{
		if(Player_life==0)
			break;

		if (packet.type == 1)
		{
			if (!strcmp(packet.buffer, "1"))
			{

			}
			else if (!strcmp(packet.buffer, "0"))
			{

			}
			else if (!strcmp(packet.buffer, "error"))
			{

			}
			else
			{
			}
		}
		else if (packet.type == 2)
		{
			for (k = 19; k >= 0; k--)
			{
				if (k == 0) // 맨 아래 채팅창의 배열에 입력된 문자열을 대입
					strcpy(Chatingchang[k], packet.buffer);
				else // 아닐경우 채팅창 배열을 한칸씩 올림
					strcpy(Chatingchang[k], Chatingchang[k - 1]);

				//현재 위치의 화면 초기화 및 이동, 맨위 부터 출력
				Clear_line(1, 20 - k);
				gotoxy(1, 20 - k);
				printf("%s", Chatingchang[k]);
			}
			gotoxy(1, 23);
		}
	}

	SetEvent(hThrd_Word);
}

void Chating(LPVOID n)
{
	int flag;
	char Player_input[101], dump;

	system("cls");
	while (1)
	{
		Clear_line(1, 23);
		gotoxy(1, 23);
		scanf("%d%c", &flag,&dump);

		switch (flag)
		{
		case 1:packet.type = 1; break;
		case 2:packet.type = 2; break;
		default:break;
		}
		Clear_line(1, 23);
		gotoxy(1, 23);
		printf("입력 : ");
		gets_s(Player_input, 30);
		
		send(hSocket, (char*)&packet, sizeof(Packet), 0);
	}
}
