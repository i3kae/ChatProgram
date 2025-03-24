#pragma once
typedef struct _packet {
    int len; // 패킷의 버퍼 길이
    std::string buffer;
}Packet;
