//원속 초기화 -> 소켓생성 -> bind->listen->accept-> 소켓해제->원속종료
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include<Windows.h>
#include<process.h>
#include<stdlib.h>
#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 300

unsigned WINAPI HandleClient(void* arg); //쓰레드 함수 
//WINAPI : __stdcall
//함수 반환값의 자료형, 함수호출규약, 매개변수의 자료형,개수 -> 함수의 원형
void SendMsg(char* msg, int len);//메세지 보내는 함수

int clientCount = 0;
SOCKET clientSocks[30]; //클라이언트 소켓 보관용 배열

typedef struct _packet {
	int type; // 1이면 단어입력, 2이면 채팅
	char buffer[100];
}Packet;

int main(int argc, char* argv[]) {
	//1. 소켓프로그래밍의 함수를 사용하려면 구조체 선언(SOCKET)
	//2. 초기화 함수를 사용하여 소켓프로그래밍 초기화(WSADATA wsa;)
	//3. 소켓을 생성하기 위해선 SOCKET sock; 구조체를 선언
	//4. 소켓 주소 구조체를 활용하여 주소 체계 확립(struct sockaddr_in sockinfo, bind)
	//5. 데이터 통신(listen,accept)
	//5-1. listen : 서버 넋놓고 있지말고 클라이언트 하는말 듣게 대기상태로 있어
	//5-2. accept : listen의 요청을 가져와서 서버 프로그램 안에 클라이언트 연결 소켓 만들기
	//6. send 함수를 이용해 문자 하나를 전송하고 종료(send,closesocket)

	SOCKET sock,clientsock;
	WSADATA wsa;
	struct sockaddr_in sockinfo, clientinfo;
	int clientsize;
	char message[] = "sucess";
	HANDLE hThread;

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
		clientSocks[clientCount++] = sock;
		//클라이언트 소켓배열에방금 가져온 소켓 주소를 전달
		_beginthreadex_proc_type

		hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClient, (void*)&sock, 0, NULL);
		//HandleClient 쓰레드 실행
	}
	closesocket(clientsock);
	closesocket(sock);
	WSACleanup();
	return 0;
}


unsigned WINAPI HandleClient(void* arg) {
	SOCKET clientSock = *((SOCKET*)arg); //매개변수로 받은 클라이언트 소켓전달
	int strLen = 0, i;
	Packet packet;
	FILE *file_dictionary, *file_note;
	file_dictionary = fopen("dictionary.txt", "rt");
	file_note = fopen("note.txt", "wt");

	while ((strLen = recv(clientSock,(char*)&packet, sizeof(packet), 0)) != -1) {
		//recv함수:반환값이 받은 데이터 크기 , 실패하면 -1
		if (packet.type == 1) {
			// 단어가 일치하는 검사 -> 단어장에 있는지 <파일1>
			// 단어 앞자리가 그 전에 왔던 뒷자리랑 같은지 <파일2>
			// 같으면?
			//		그 단어를 반환
			// 다르면?
			//		0
			// 중복되면?
			//		1

		}
		else if (packet.type == 2) {
			SendMsg((char*)&packet, strLen);
		}
		else {
			strcpy(packet.buffer, "error");
			SendMsg((char*)&packet, strLen);
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
		send(clientSocks[i],packet->buffer, len, 0);

}