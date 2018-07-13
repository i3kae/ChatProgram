//원속 초기화 -> 소켓생성 -> bind->listen->accept-> 소켓해제->원속종료
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#pragma comment(lib,"ws2_32.lib")


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

	clientsock = accept(sock, (SOCKADDR*)&clientinfo, &clientsize);
	//accept(서버 소켓,((SOCKADDR*)&클라이언트주소값,&클라이언트 주소값 사이즈))
	//클라이언트와 연결 될 소켓을 만들어 주어야 하므로 clientsock변수 선언

	if (clientsock == INVALID_SOCKET)
	{
		printf("클라이언트 소켓 연결 실패");
		return 0;
	}

	send(clientsock, message, sizeof(message), 0);

	closesocket(clientsock);
	closesocket(sock);

	printf("성공\n");

	WSACleanup();

	return 0;
}