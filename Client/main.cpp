#include "utils.h"
#include "client.h"


int main(int argc, char* argv[]) {
	string ip = "127.0.0.1", port = "5959";
	#ifndef _DEBUG
		if (argc != 3) {
			printf("Usage : %s <IP> <port>\n", argv[0]);
			exit(1);
		}
		ip = argv[1];
		port = argv[2];
	#endif

	Client client = Client(port, ip);
	client.serverConnect();
	return 0;
}