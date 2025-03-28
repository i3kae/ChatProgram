//원속 초기화 -> 소켓생성 -> bind->listen->accept-> 소켓해제->원속종료
#define _CRT_SECURE_NO_WARNINGS
#include "utils.h"
#include "server.h"

#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 300

using namespace std;

vector<string> ReadWordList(string filePath);

int main(int argc, char* argv[]) {
	// 1. 소켓프로그래밍의 함수를 사용하려면 구조체 선언(SOCKET)
	// 2. 초기화 함수를 사용하여 소켓프로그래밍 초기화(WSADATA wsa;)
	// 3. 소켓을 생성하기 위해선 SOCKET sock; 구조체를 선언
	// 4. 소켓 주소 구조체를 활용하여 주소 체계 확립(struct sockaddr_in sockinfo, bind)
	// 5. 데이터 통신(listen,accept)
	// 5-1. listen : 서버 넋놓고 있지말고 클라이언트 하는말 듣게 대기상태로 있어
	// 5-2. accept : listen의 요청을 가져와서 서버 프로그램 안에 클라이언트 연결 소켓 만들기
	// 6. send 함수를 이용해 문자 하나를 전송하고 종료(send,closesocket)

	vector<string> wordList = ReadWordList("../Constant/dictionary.txt");
	
	Server server{ 5959, wordList };
	server.serverStart(SOMAXCONN);

	return 0;
}