#pragma once

#include <string>

typedef struct _packet {
	int type; // 1�̸� �ܾ��Է�, 2�̸� ä��
	std::string buffer;
}Packet;