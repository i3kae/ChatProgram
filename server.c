//원속 초기화 -> 소켓생성 -> bind->listen->accept-> 소켓해제->원속종료
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include<Windows.h>
#include<process.h>
#include<stdlib.h>
#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 300

char dictionary_list[1000000][30];
int dictionary_count = 0;
char word_list[1000000][30];
int word_count = 0, flag = 0;
void ReadWordList();

typedef struct _packet {
	int type; // 1이면 단어입력, 2이면 채팅
	char buffer[100];
}Packet;

unsigned WINAPI HandleClient(void* arg); //쓰레드 함수 
										 //WINAPI : __stdcall
										 //함수 반환값의 자료형, 함수호출규약, 매개변수의 자료형,개수 -> 함수의 원형

void SendMsg(Packet* packet, int len);//메세지 보내는 함수
void Word_Check(char *input, char *prev_word);

int clientCount = 0;
SOCKET clientSocks[30]; //클라이언트 소켓 보관용 배열


int main(int argc, char* argv[]) {
	//1. 소켓프로그래밍의 함수를 사용하려면 구조체 선언(SOCKET)
	//2. 초기화 함수를 사용하여 소켓프로그래밍 초기화(WSADATA wsa;)
	//3. 소켓을 생성하기 위해선 SOCKET sock; 구조체를 선언
	//4. 소켓 주소 구조체를 활용하여 주소 체계 확립(struct sockaddr_in sockinfo, bind)
	//5. 데이터 통신(listen,accept)
	//5-1. listen : 서버 넋놓고 있지말고 클라이언트 하는말 듣게 대기상태로 있어
	//5-2. accept : listen의 요청을 가져와서 서버 프로그램 안에 클라이언트 연결 소켓 만들기
	//6. send 함수를 이용해 문자 하나를 전송하고 종료(send,closesocket)

	SOCKET sock, clientsock;
	WSADATA wsa;
	struct sockaddr_in sockinfo, clientinfo;
	int clientsize;
	char message[] = "sucess";
	HANDLE hThread;

	ReadWordList();

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) //초기화
	{
		printf("초기화 실패\n");
		return 0;
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//sock=socket(af,type,protocol);
	//af=주소체계 , type=소켓타입 , protocol=프로토콜
	//인터넷 영역을 사용하는 TCP방식의 프로토콜
	if (sock == INVALID_SOCKET)
	{
		printf("소켓 생성 실패\n");
		return 0;
	}

	memset(&sockinfo, 0x00, sizeof(sockinfo));
	sockinfo.sin_family = AF_INET; //sockinfo 구조체의 sin_family에 인터넷 주소 사용
	sockinfo.sin_port = htons(5959); //포트 지정
	sockinfo.sin_addr.s_addr = htonl(INADDR_ANY); //0.0.0.0주소를 사용하여 
												  //모든 사용가능한 주소로부터 대기

	if (bind(sock, (SOCKADDR*)&sockinfo, sizeof(sockinfo)) == SOCKET_ERROR)
	{
		//int bind (소켓 값,(SOCKADDR*)&소켓주소구조체 저장값,sizeof(소켓구조체 저장값))
		//bind 주소를 사용하여 소켓 구조체의 정보를 진정한 소켓에 전달
		printf("bind 실패");
		return 0;
	}


	if (listen(sock, 5) == SOCKET_ERROR)
	{
		printf("대기열 실패\n");
		return 0;
	}
	//int listen(SOCKET,backlog)
	//SOCKET :소켓 생성시 만들어서 저장했던 소켓 핸들 값
	//BAcklog : 통로값

	clientsize = sizeof(clientinfo);
	printf("클라이언트로부터 접속을 기다리고 있습니다...\n");

	while (1)
	{
		clientsock = accept(sock, (SOCKADDR*)&clientinfo, &clientsize);
		//accept(서버 소켓,((SOCKADDR*)&클라이언트주소값,&클라이언트 주소값 사이즈))
		//클라이언트와 연결 될 소켓을 만들어 주어야 하므로 clientsock변수 선언

		if (clientsock == INVALID_SOCKET)
		{
			printf("클라이언트 소켓 연결 실패");
			return 0;
		}
		clientSocks[clientCount++] = clientsock;
		//클라이언트 소켓배열에방금 가져온 소켓 주소를 전달

		hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClient, (void*)&clientsock, 0, NULL);
		//HandleClient 쓰레드 실행
	}
	closesocket(clientsock);
	closesocket(sock);
	WSACleanup();
	return 0;
}


void ReadWordList()
{
	FILE *file_dictionary;
	char buffer[31];
	file_dictionary = fopen("dictionary.txt", "rt");
	if (file_dictionary == NULL)
	{
		printf("File error");
	}
	//////////
	//fscanf의 반환값은 성공적으로 읽힌 변수의 개수
	//굉장히 많이 쓰이는 트릭임!
	while (fscanf(file_dictionary, "%s", buffer) == 1) {
		strcpy(dictionary_list[dictionary_count++], buffer);
	}
	//file_dictionary에서 읽어오는 즉시 word_list에 복사
	//////////
	fclose(file_dictionary);
}

unsigned WINAPI HandleClient(void* arg) {
	SOCKET clientSock = *((SOCKET*)arg); //매개변수로 받은 클라이언트 소켓전달
	int strLen = 0, i = 0, j = 0, isIn = 0;
	char word[30], sameword[30], prev_word[31];
	Packet packet;

	while (recv(clientSock, (char*)&packet, sizeof(Packet), 0) != -1) {
		//recv함수:반환값이 받은 데이터 크기 , 실패하면 -1
		//지금 받은 단어는 packet에 있음
		if (packet.type == 1) {
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
			for (int i = 0; i < dictionary_count; i++)
			{
				if (strcmp(dictionary_list[i], packet.buffer) == 0)
				{
					// 기존에 있었던거 ? -> 1
					// 없던거 -> 단어반환
					isIn = 1;
					break;
				}
			}

			if (isIn)
			{
				//word 검사해서 있으면 1
				// 없으면 단어
				isIn = 0;
				for (int i = 0; i < word_count; i++) {
					if (strcmp(word_list[i], packet.buffer) == 0) {
						isIn = 1;
						break;
					}
				}
				if (isIn)
				{
					strcpy(packet.buffer, "1");
				}
				else
				{
					strcpy(word_list[word_count], packet.buffer);
					strcpy(prev_word, packet.buffer);
					word_count++;
				}
			}
			else
			{
				strcpy(packet.buffer, "0");
				// 0
			}

			// 위에서 단어장에 없는 단어면 -> 0
			//앞, 뒤 글자가 같은지 체크 첫 번째는 무조건 넘겨줌
			if (flag != 0)
			{
				if ((prev_word[strlen(prev_word) - 1] != packet.buffer)&&(strcmp(packet.buffer,"0"))&& (strcmp(packet.buffer, "0")))
				{
					strcpy(packet.buffer, "-1");
				}
			}
			if ((strcmp(packet.buffer, "0")) && (strcmp(packet.buffer, "1")) && (flag == 0))
				flag = 1;
			packet.type = 3;
			send(clientSock, (char*)&packet, sizeof(Packet), 0);
			packet.type = 1;
			Sleep(10);
			SendMsg(&packet, strLen);
		}
		else if (packet.type == 2) {
			SendMsg(&packet, strLen);
		}
		else {
			strcpy(packet.buffer, "error");
			SendMsg(&packet, strLen);
		}
	}
	//이 줄을 실행한다는 것은 해당 클라이언트가 나갔다는 사실
	//따라서 해당 클라이언트를 배열에서 빼줘야한다
	for (i = 0; i < clientCount; i++) { //배열의 갯수만큼
		if (clientSock == clientSocks[i]) { //만약 현재 clientSocks값이 배열의 값과 같다면
			while (i++ < clientCount - 1)//클라이언트 개수만큼
				clientSocks[i] = clientSocks[i + 1]; //앞으로 땡긴다
			break;
		}
	}
	clientCount--;
	closesocket(clientSock);//소켓을 종료한다
	return 0;
}

void SendMsg(Packet* packet, int len) { //메세지를 모든 클라이언트에 보낸다
	int i;
	for (i = 0; i < clientCount; i++)
	{
		send(clientSocks[i], (char*)packet, sizeof(Packet), 0);
	}
}

void Word_Check(char *input, char *prev_word)
{
	int i = 0, flag = 0;
	char file_load[100];
	char Player_input[32];
	FILE *file_pointer;
	file_pointer = fopen("Word.txt", "rt");

	//사전 파일이 없을경우
	if (file_pointer == NULL)
	{
		return -1;
	}

	//사전을 검색 중 단어가 있으면 0을 리턴
	while (!feof(file_pointer))
	{
		fgets(file_load, 99, file_pointer); //파일에서 한줄씩 입력
		if (!strcmp(file_load, Player_input)) //파일에있는 문자열중 하나라고 Player_input과 같은 것이 있으면
		{
			return 0;
		}
	}

	//없으면 1을 리턴
	return 1;
}
