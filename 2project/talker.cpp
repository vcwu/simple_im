
#define WSVERS MAKEWORD(2,0)
#define DEBUG

#include "MySock.h"
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <string>
#include <winsock.h>

#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "libcmt.lib")


using namespace std;

int main(int argc, char ** argv)	{
	
	string serverName, portNum;
	if(argc ==3)	{
		serverName = argv[1];
		portNum = argv[2];
	}
	else	{
		cout << "Need server name and ip." << endl;
		return -1;
	}
	
	WSADATA wsadata;
	if(WSAStartup(WSVERS, &wsadata) != 0)	{
		std::cerr << "WSA Startup failed " << std::endl;
		exit(1);
	}
	

	MySock cheese;
	cheese.connectToHost(serverName, portNum);
	cheese.sendMsg("ELLO MY FRIEND");
	WSACleanup();
}
