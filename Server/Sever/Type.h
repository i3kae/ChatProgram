#pragma once
typedef struct _packet {
    int type; // 1이면 단어입력, 2이면 채팅
    char buffer[100];
}Packet;
