#include <iostream>
#include <Windows.h>
#include "utils.h"

using namespace std;

//x,y위치 화면 초기화
void Clear_line(int x, int y)
{
	gotoxy(x, y);
	cout << "                                        ";
}

//커서 이동 함수
void gotoxy(int x, int y)
{
	COORD pos = { x - 1, y - 1 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}