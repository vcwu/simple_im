
#define WSVERS MAKEWORD(2,0)
#define DEBUG

#include "MySock.h"
#include <sstream>
#include <cstdlib>
#include <iostream>

#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "libcmt.lib")


using namespace std;

int main()	{
	
	WSADATA wsadata;
	if(WSAStartup(WSVERS, &wsadata) != 0)	{
		std::cerr << "WSA Startup failed " << std::endl;
		exit(1);
	}
	
	MySock yo;
	yo.startListening(10, 34567);

	struct sockaddr their_addr;
	int addr_len = sizeof(their_addr);
	while(true)	{
		SOCKET fd;
		fd = accept(yo.getSocket(), &their_addr, &addr_len);
		MySock slave(fd);
		slave.sendMsg("Howdy there!");		
	}
	WSACleanup();
}
