#pragma once
#include <list>
#include <string>
#include <iostream>
#include "utils.h"
#include "constant.h"

using namespace std;

class ChatWindow {
private:
	list<string> chatWindow;

public:
	void updateChat(string newChat);
	void printChat();
};