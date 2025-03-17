#include <iostream>
#include <vector>
#include "utils.h"

using namespace std;

vector<string> ReadWordList(string filePath)
{
	vector<string> wordList;
	FILE* fileDictionary;
	char buffer[30];

	fileDictionary = fopen(filePath.c_str(), "rt");
	if (fileDictionary == NULL)
	{
		printf("File error");
		return vector<string>(0);
	}
	//////////
	// fscanf�� ��ȯ���� ���������� ���� ������ ����
	// ������ ���� ���̴� Ʈ����!
	while (fscanf(fileDictionary, "%s", buffer) == 1) {
		wordList.emplace_back(buffer);
	}
	// fileDictionary���� �о���� ��� wordList�� ����
	//////////
	fclose(fileDictionary);

	return wordList;
}
