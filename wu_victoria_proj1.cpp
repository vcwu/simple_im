#include <winsock.h>
#include <stdio.h>

int main(int argc, char **argv)	{
	



	char* serverName;
	char* portNum;

	//Take in user specifed port num and server name.
	if(argc == 2)	{
		serverName = argv[1];
		portNum = argv[2];
	}
	else	{
		printf("Need server name and port number.");
		exit(1);
	}

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

