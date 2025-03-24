#include <list>
#include <string>
#include <iostream>
#include "utils.h"
#include "constant.h"
#include "chatWindow.h"

using namespace std;

void ChatWindow::updateChat(string newChat) {
	if (chatWindow.size() >= MAX_CHAT_COUNT) {
		chatWindow.pop_back();
		chatWindow.push_front(newChat);
	}
	else
		chatWindow.push_front(newChat);
}

void ChatWindow::printChat() {
	int count = MAX_CHAT_COUNT;
	for (list<string>::iterator iter = chatWindow.begin(); iter != chatWindow.end(); iter++, count--) {
		Clear_line(1, count);
		gotoxy(1, count);
		cout << *iter;
	}
	gotoxy(8, 23);
}