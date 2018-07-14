#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN
#define PRINT_WORD_CHAIN_X 50
DWORD WINAPI ThreadFunc(LPVOID);
// DWORD는 더블워드 == long
// WINAPI 윈도우에서의 API
//LPVOID ms에서 사용하는 void포인터

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

//끝말잇기와 채팅의 이벤트와 쓰레드정보를 담을 변수
HANDLE hThrd_Word, hThrd_Chating;
HANDLE WORD_EVENT, CHATING_EVENT;
DWORD threadId_Word, threadId_Chating;

//플레이어의 정보를 담을 구조체
typedef struct player
{
	char name[21];
	int life;
	int score;
}Player_Information;

//함수 원형 선언
void Word_Chain(LPVOID n);
int Word_Check(char *input);
void Chating(LPVOID n);
void gotoxy(int x, int y);
void Clear_line(x, y);


int main(int argc, char **argv)
{
	int i;

	//쓰레드를 생성
	hThrd_Word = CreateThread(NULL, 0, (DWORD WINAPI)Word_Chain, 0, 0, &threadId_Word);
	hThrd_Chating = CreateThread(NULL, 0, (DWORD WINAPI)Chating, 0, 0, &threadId_Chating);

	//쓰레드를 정지 시키고 실행할 이벤트 설정
	WORD_EVENT = CreateEvent(NULL, TRUE, FALSE, NULL);
	CHATING_EVENT = CreateEvent(NULL, TRUE, FALSE, NULL);

	//끝말잇기 함수가 끝나면 프로세스 종료
	WaitForSingleObject(hThrd_Word, INFINITE);
}

void Word_Chain(LPVOID n)
{
	Player_Information *Players;
	int player_count, i, j, k, flag = 0, word_flag = 0, input_flag = 0, Chain = 0;
	char dump, end_word, first_word;
	char Player_input[101], Prev_input[101];

	// 로컬 전용 //

	//플레이어의 수 입력
	gotoxy(PRINT_WORD_CHAIN_X, 4);
	printf("다함께 끝말잇기 - Beta");
	gotoxy(PRINT_WORD_CHAIN_X, 5);
	printf("플레이어 수를 입력해 주세요 : ");
	scanf("%d", &player_count);
	scanf("%c", &dump);

	Clear_line(PRINT_WORD_CHAIN_X, 4);
	Clear_line(PRINT_WORD_CHAIN_X, 5);

	//플레이어 수 만큼 배열 할당
	Players = (Player_Information*)malloc(player_count * sizeof(Player_Information));

	//플레이어 라이프 3으로 초기화
	for (i = 0; i < player_count; i++)
		Players[i].life = 3;

	//플레이어 이름 입력
	for (i = 0; i < player_count; i++)
	{
		Clear_line(PRINT_WORD_CHAIN_X, 15);
		gotoxy(PRINT_WORD_CHAIN_X, 15);
		printf("플레이어 %d의 이름을 입력해 주세요 : ", i + 1);
		gets_s(Players[i].name, 20);
	}

	////

	//끝말잇기 부분
	while (1)
	{

		//플레이어 단어 입력
		for (i = 0; i < player_count; i++)
		{
			input_flag = 0;

			//플레이어 라이프가 0 일경우 입력 단계 스킵
			if (!Players[i].life)
				continue;

			//전 입력 단어 출력
			if (Chain != 0)
			{
				Clear_line(PRINT_WORD_CHAIN_X, 12);
				gotoxy(PRINT_WORD_CHAIN_X, 12);
				printf("전 입력 단어 : %s", Prev_input);
			}
			//플레이어 단어 입력
			Clear_line(PRINT_WORD_CHAIN_X, 15);
			gotoxy(PRINT_WORD_CHAIN_X, 15);

			printf("플레이어 %s님 입력해주세요 : ", Players[i].name);

			//WORD_EVENT를 설정함으로 Chating함수를 실행 시키고 CHATING_EVENT를 초기화 한 후 CHATING_EVENT신호가 올때까지 대기  
			SetEvent(WORD_EVENT);
			ResetEvent(CHATING_EVENT);
			WaitForSingleObject(CHATING_EVENT, INFINITE);

			//입력한 문자열의 맨앞부분이 !면 끝말잇기로 처리 아니면 채팅으로 처리
			if (Chat[0] == '!')
			{
				if (Chain != 0) // 첫 유저의 경우 아무 단어나 입력 가능
				{
					//전 입력 끝 단어와 현 입력 첫 단어 비교
					if (Chat[1] != Prev_input[strlen(Prev_input) - 2])
					{
						Clear_line(PRINT_WORD_CHAIN_X, 9);
						gotoxy(PRINT_WORD_CHAIN_X, 9);
						printf("전 단어의 끝 단어와 현 단어의 첫 단어가 다릅니다.");
						Players[i].life--;
						input_flag = 1;
						continue;
					}
				}

				//플레이어 단어 체크
				word_flag = Word_Check(Chat, Prev_input);

				if (word_flag == 1) // 단어가 사전에 없을 경우 
				{
					Players[i].life--;
					input_flag = 1;
				}
				else if (word_flag == -1) // 사전이 없을 경우
					SetEvent(hThrd_Word);

				if (input_flag == 0) // 단어가 있을경우 전 단어 배열에 입력단어 대입
				{
					for (k = 1; k < 32; k++)
					{
						Prev_input[k - 1] = Chat[k];
					}
					Chain = 1;
				}

				//플레이어들의 라이프 상태 체크
				for (j = 0, flag = 0; j < player_count; j++)
				{
					if (Players[j].life == 0)
						flag++;
				}

				if (flag == player_count - 1) // 한명을 제외한 모든 플레이어의 라이프가 0일 경우
					break;
			}
			else // 채팅 출력부분
			{
				for (k = 19; k >= 0; k--)
				{
					if (k == 0) // 맨 아래 채팅창의 배열에 입력된 문자열을 대입
					{
						strcpy(Chatingchang[k], Players[i].name);
						strcat(Chatingchang[k], " : ");
						strcat(Chatingchang[k], Chat);
					}
					else // 아닐경우 채팅창 배열을 한칸씩 올림
					{
						strcpy(Chatingchang[k], Chatingchang[k - 1]);
					}

					//현재 위치의 화면 초기화 및 이동, 맨위 부터 출력
					Clear_line(1, 20 - k);
					gotoxy(1, 20 - k);

					printf("%s", Chatingchang[k]);
				}

				i--; //채팅을 친 플레이어의 차례가 다시 돌아오도록
			}
		}

		//플레이어들의 라이프 상태 체크
		for (j = 0, flag = 0; j < player_count; j++)
		{
			if (Players[j].life == 0)
				flag++;
		}

		if (flag == player_count - 1) // 한명을 제외한 모든 플레이어의 라이프가 0일 경우
			break;

	}

	//life가 0이 아닌 플레이어 탐색
	for (i = 0; i < player_count; i++)
	{
		if (Players[i].life != 0)
		{
			Clear_line(PRINT_WORD_CHAIN_X, 15);
			gotoxy(PRINT_WORD_CHAIN_X, 15);
			printf("플레이어 %s님이 승리 하였습니다!", Players[i].name);
			break;
		}
	}

	//이 쓰레드가 끝났다고 알림
	SetEvent(hThrd_Word);
}

//단어 체크 함수
int Word_Check(char *input, char *prev_word)
{
	int i = 0, flag = 0;
	char file_load[100];
	char Player_input[32];
	FILE *file_pointer;
	file_pointer = fopen("Word.txt", "rt");

	//사전 파일이 없을경우
	if (file_pointer == NULL)
	{
		Clear_line(PRINT_WORD_CHAIN_X, 9);
		gotoxy(PRINT_WORD_CHAIN_X, 9);
		printf("사전 파일이 없습니다. Word파일을 설정후 이용해 주세요.");
		return -1;
	}

	//입력된 문자열에서 !를 제거
	for (i = 1; i < 32; i++)
		Player_input[i - 1] = input[i];

	//사전을 검색 중 단어가 있으면 0을 리턴
	while (!feof(file_pointer))
	{
		fgets(file_load, 99, file_pointer); //파일에서 한줄씩 입력
		if (!strcmp(file_load, Player_input)) //파일에있는 문자열중 하나라고 Player_input과 같은 것이 있으면
		{
			Clear_line(PRINT_WORD_CHAIN_X, 9);
			gotoxy(PRINT_WORD_CHAIN_X, 9);
			printf("단어가 존재합니다.");
			return 0;
		}
	}

	//없으면 1을 리턴
	Clear_line(PRINT_WORD_CHAIN_X, 9);
	gotoxy(PRINT_WORD_CHAIN_X, 9);
	printf("단어가 없습니다");
	return 1;
}

//커서 이동 함수
void gotoxy(int x, int y)
{
	COORD pos = { x - 1, y - 1 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

//x,y위치 화면 초기화
void Clear_line(x, y)
{
	gotoxy(x, y);
	printf("                                                            ");
}

//채팅 입력
void Chating(LPVOID n)
{
	FILE *fp;

	fp = fopen("Chating_Log.txt", "w"); // 파일을 온리 쓰기형으로 염

	if (fp == NULL)
	{
		printf("파일 읽기 오류");
		return;
	}

	//채팅 입력 부분
	while (1)
	{
		//WORD_EVENT이벤트를 초기화하고 WORD_EVENV에 신호가 올떄까지 대기후
		ResetEvent(WORD_EVENT); //WORD_EVENT이벤트를 초기화
		WaitForSingleObject(WORD_EVENT, INFINITE); //WORD_EVENT이벤트에 신호가 올떄까지 대기
		gotoxy(PRINT_WORD_CHAIN_X + 28, 15);
		fgets(Chat, 30, stdin);
		fprintf(fp, "%s", Chat); // 파일에 입력
		SetEvent(CHATING_EVENT); // CHATRING_EVENT로 신호를 보냄
	}

	fclose(fp); //fp파일을 닫아줌
	SetEvent(hThrd_Chating); //현재 쓰레드가 끝났다고 알림
}