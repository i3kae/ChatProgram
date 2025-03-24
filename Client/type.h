#pragma once

#include <string>

typedef struct _packet {
	int len; // 패킷 버퍼의 길이
	std::string buffer;
}Packet;