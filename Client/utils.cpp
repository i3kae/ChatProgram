#include <iostream>
#include <Windows.h>
#include "utils.h"

using namespace std;

//x,y��ġ ȭ�� �ʱ�ȭ
void Clear_line(int x, int y)
{
	gotoxy(x, y);
	cout << "                                        ";
}

//Ŀ�� �̵� �Լ�
void gotoxy(int x, int y)
{
	COORD pos = { x - 1, y - 1 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}