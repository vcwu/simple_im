#include <winsock.h>
#include <iostream>	//cerr

#pragma comment(lib, "wsock32.lib")	//link winsock lib
#define WSVERS MAKEWORD(2,0)

void UDP_IM_Client(const char* , const char* );

int main(int argc, char **argv)	{
	
	char* serverName;
	char* portNum;
	WSADATA wsadata;

	//Take in user specifed port num and server name.
	if(argc == 2)	{
		serverName = argv[1];
		portNum = argv[2];
	}
	else	{
		std::cerr << "Need server name and port number.";
		exit(1);
	}

	if(WSAStartup(WSVERS, &wsadata) != 0)	{
		std::cerr << "WSAStartup failed. ";
		exit(1);
	}

	UDP_IM_Client(serverName, portNum);

	WSACleanup();
	return 0;
	//Allocate socket, find unused port and bind.
//	SOCKET s;
//	s = socket(PF_INET, SOCK_DGRAM,"UDP");

	//Connect socket to server.
	
	//Application time!
	//
	//Start thread to monitor connection to server.
	//Present user with dialog for choices.
	//On Quit, close connection.
}

void UDP_IM_Client(const char* serverName, const char* portNum)	{
	std::cout << "Yohoo!"<< std::endl;
}
