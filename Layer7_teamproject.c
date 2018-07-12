#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

typedef struct player
{
	char name[21];
	int life;
	int score;
}Player_Information;

//함수 원형 선언
void Word_Chain();
int Word_Check(char *input);


int main(int argc, char **argv)
{
	Word_Chain();
}

void Word_Chain()
{
	Player_Information *Players;
	int player_count, i, j, k, flag = 0, word_flag = 0, input_flag = 0, Chain = 0;
	char dump,end_word,first_word;
	char Player_input[101], Prev_input[101];

	//플레이어의 수 입력
	printf("다함께 끝말잇기 - Beta\n\n");
	printf("플레이어 수를 입력해 주세요 : ");
	scanf("%d", &player_count);
	scanf("%c", &dump);

	//플레이어 수 만큼 배열 할당
	Players = (Player_Information*)malloc(player_count * sizeof(Player_Information));

	//플레이어 라이프 3으로 초기화
	for (i = 0; i < player_count; i++)
		Players[i].life = 3;

	//플레이어 이름 입력
	for (i = 0; i < player_count; i++)
	{
		printf("플레이어 %d의 이름을 입력해 주세요 : ", i + 1);
		gets_s(Players[i].name, 20);
	}

	//끝말잇기 부분
	while (1)
	{
		input_flag = 0;

		//플레이어 단어 입력
		for (i = 0; i < player_count; i++)
		{
			//플레이어 라이프가 0 일경우 입력 단계 스킵
			if (!Players[i].life)
				continue;

			//플레이어 단어 입력
			printf("\n플레이어 %s님 입력해주세요 : ", Players[i].name);

			fgets(Player_input, 100, stdin);

			if (Chain != 0)
			{
				//전 입력 끝 단어와 현 입력 첫 단어 비교
				if (Player_input[0] != Prev_input[strlen(Prev_input) - 2])
				{
					printf("\n전 단어의 끝 단어와 현 단어의 첫 단어가 다릅니다.\n");
					Players[i].life--;
					input_flag = 1;
					continue;
				}
			}

			//플레이어 단어 체크
			word_flag = Word_Check(Player_input);

			if (word_flag == 1) // 단어가 사전에 없을 경우 
			{
				Players[i].life--;
				input_flag = 1;
			}
			else if (word_flag == -1) // 사전이 없을 경우
				return;

			if (input_flag == 0)
			{
				strcpy(Prev_input, Player_input);
				Chain = 1;
			}
		}

		//플레이어들의 라이프 상태 체크
		for (i = 0,flag=0; i < player_count; i++)
		{
			if (Players[i].life == 0)
				flag++;
		}

		if (flag == player_count - 1) // 모든 플레이어의 라이프가 0일 경우
			break;

	}

	for (i = 0; i < player_count; i++)
	{
		if (Players[i].life != 0)
		{
			printf("\n플레이어 %s님이 승리 하였습니다!\n", Players[i].name);
			break;
		}
	}
}

//단어 체크 함수
int Word_Check(char *input)
{
	int i = 0 , flag = 0;
	char file_load[100];
	FILE *file_pointer;
	file_pointer = fopen("Word.txt", "rt");

	if (file_pointer == NULL)
	{
		printf("\n사전 파일이 없습니다. Word파일을 설정후 이용해 주세요.\n");
		return -1;
	}

	while (!feof(file_pointer))
	{
		fgets(file_load, 99, file_pointer);
		if (!strcmp(file_load, input))
		{
			printf("\n단어가 존재합니다.\n");
			return 0;
		}
	}
	printf("\n단어가 없습니다\n");
	return 1;
}

