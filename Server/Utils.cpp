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
	// fscanf의 반환값은 성공적으로 읽힌 변수의 개수
	// 굉장히 많이 쓰이는 트릭임!
	while (fscanf(fileDictionary, "%s", buffer) == 1) {
		wordList.emplace_back(buffer);
	}
	// fileDictionary에서 읽어오는 즉시 wordList에 복사
	//////////
	fclose(fileDictionary);

	return wordList;
}
