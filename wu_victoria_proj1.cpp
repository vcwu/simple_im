#include <process.h>	//multi threading
#include <winsock.h>
#include <iostream>	//cerr

#pragma comment(lib, "wsock32.lib")	//link winsock lib
#pragma comment(lib, "libcmt.lib")	//for process.h
#define WSVERS MAKEWORD(2,0)

SOCKET connectsock(const char*, const char*);
void UDP_IM_Client(const char* , const char* );
void listener(void* socketNum);		//monitor for messages from server

int main(int argc, char **argv)	{
	
	char* serverName;
	char* portNum;
	WSADATA wsadata;

	//Take in user specifed port num and server name.
	if(argc == 3)	{
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

	while(true)	{
	}

	WSACleanup();
	return 0;

	
	
}

void UDP_IM_Client(const char* serverName, const char* portNum)	{

	std::cout << "Yohoo!"<< std::endl;

	SOCKET s;
	s = connectsock(serverName, portNum);

	//Start thread to monitor connection to server.
	_beginthread(listener, 0, (void*) &s);	
	
	//Sign on to server.
	
		
	//on quit, exit
	closesocket(s);
}

/*
 * listener
 * monitors and displays messages from server.
 *
 * @param socketNum
 */
void listener(void* socketNum)	{
	std::cout << "Listening for server messages..." <<std::endl;
}

/*
 * connectsock - allocate and connect socket.
 * modified from Comer's Internetworking with TCP/IP Volume III, section 7.7
 *
 * @param serverName server IP address in dotted-decimal format
 * @param portNum	
 */
SOCKET connectsock(const char* serverName, const char* portNum)	{
	const char* transport = "udp";
	int s, type;	//socket descriptor and socket type

	struct sockaddr_in sin;	// an internet endpoint address
	struct protoent *ppe;	// pointer to protocol information entry

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons((u_short)atoi(portNum)); 
	sin.sin_addr.s_addr = inet_addr(serverName);
	
	//map protocol name to protocol number, define type
	ppe = getprotobyname(transport);
	if(strcmp(transport, "udp") == 0 )	
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;

	//Finally, allocate a socket.
	s = socket(PF_INET, type, ppe->p_proto);
	if(s == INVALID_SOCKET)	{
		std::cerr << "can't create socket";
		WSACleanup();
		exit(1);
		
	}

	//Connect socket.
	if(connect(s, (struct sockaddr*) &sin, sizeof(sin))==SOCKET_ERROR) 	
	{
		std::cerr << "can't connect to " << serverName << " " << portNum;
		WSACleanup();
		exit(1);
	}

	std::cout << "Socket Creation and connection successful" <<std::endl;
	return s;
}
