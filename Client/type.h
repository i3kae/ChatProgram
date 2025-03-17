#pragma once

#include <string>

typedef struct _packet {
	int type; // 1이면 단어입력, 2이면 채팅
	std::string buffer;
}Packet;