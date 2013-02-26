#ifndef DEBUG
#define DEBUG 1
#endif


#include <process.h>	//multi threading
#include <winsock.h>	
#include <iostream>	//cerr
#include <sstream>	
#include <string>
#include <cstdlib>	//rand, srand
#include <time.h>	//srand time

#pragma comment(lib, "wsock32.lib")	//link winsock lib
#pragma comment(lib, "libcmt.lib")	//for process.h
#define WSVERS MAKEWORD(2,0)

SOCKET connectsock(const char*, const char*);
void UDP_IM_Client(const char* , const char* );
void listener(void* socketNum);		//monitor for messages from server

/*
 * IM Client Functions
 */
void signIn(int& msgNum, std::string name, SOCKET s);
void logOut(int& msgNum, std::string name, SOCKET s);


int main(int argc, char **argv)	{
	
	char* serverName;
	char* portNum;
	WSADATA wsadata;

	//Take in user specifed port num and server name.
	if(argc == 3)	{
		serverName = argv[1];
		portNum = argv[2];
		std::cout << "Server IP: " << serverName << std::endl;
		std::cout << "Port Num : " << portNum << std::endl;
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

	

	std::cout << "Goodbye!" << std::endl;
	WSACleanup();
	return 0;

	
	
}

void UDP_IM_Client(const char* serverName, const char* portNum)	{


	//Generate random num for message sequencing.
	srand(time(NULL));
	//int msgNum = rand() % 1000 + 10000;
	int msgNum = 12345;
	if(DEBUG)	
		std::cout << "Generating msg num " << msgNum <<std::endl;
	
	//Connect to server.	
	SOCKET s;
	s = connectsock(serverName, portNum);
	if(DEBUG)
		std::cout << "Connected to socket " << s << std::endl;

	//Start thread to monitor connection to server.
	_beginthread(listener, 0, (void*) &s);	
	
	//Sign on to server.
	std::string name;
	std::cout << "Enter your IM name: ";
	std::cin >> name;
		signIn(msgNum,name, s);signIn(msgNum,name, s);

	//Do stuff
	//
	
	std::string input;
	while(true)	{
		std::cin >> input;
		if(input == "q")	
			break;
	}		
	
	//Sign off.
	logOut(msgNum, name, s);	

	//on quit, exit
	closesocket(s);
}

/*
 * signIn
 * signs into server.
 * @param name user's desired handle
 * @param msgNum sequential msg num
 * @param s socket num
 */
void signIn(int& msgNum, std::string name, SOCKET s)	{
	std::cout << "Signing in as " << name << std::endl;
	
	char num[10];
	itoa(msgNum, num, 10);

	std::cout << "MSG NUM : " << num << std::endl;

	char buf[256];
       	buf[256] = '\0';
	std::stringstream ss;

	ss << num << ";1;" << name;

	std::string message = ss.str();
	
	

	int bytes_sent = send(s, message.c_str(), strlen(message.c_str() ), 0);
	if(bytes_sent == SOCKET_ERROR)	{
		std::cerr << "error in sending sign in msg" << std::endl;	
	}

	if(DEBUG)	{
		std::cout << "Sent " << bytes_sent << "bytes. " << std::endl;	
		std::cout << "Message: " << message.c_str() << std::endl;
	}
	msgNum++;
}

/*
 * logOut
 * log out of server.
 * @param s socket num
 * @param msgNum sequential msg number
 */

void logOut(int& msgNum, std::string name, SOCKET s)	{
	std::cout << "Logging off... " << std::endl;
	
	char num[5];
	itoa(msgNum, num, 10);
	std::stringstream ss;

	ss << num << ";3;" << name;

	std::string message = ss.str();

	int bytes_sent = send(s, message.c_str(), sizeof(message.c_str()), 0);
	if(bytes_sent == SOCKET_ERROR)	{
		std::cerr << "Error in sending log off msg" << std::endl;
	}
	if(DEBUG)	{
		std::cout << "Sent: " << bytes_sent << "bytes." <<std::endl;
		std::cout << "Msg: " << message << std::endl;
	}
}

/*
 * listener
 * monitors and displays messages from server.
 *
 * @param socketNum
 */
void listener(void* socketNum)	{
	if(DEBUG)
		std::cout << "Listening for server messages..." <<std::endl;
	
	SOCKET s = *( (SOCKET*)socketNum);
	if(DEBUG)
		std::cout << "Listening to socket " << s << std::endl;
	const int bufferLength = 200;
	char recvbuf[bufferLength];
	
	while(true)	{
		if( recv(s, recvbuf, bufferLength, 0) == SOCKET_ERROR)	{
			std::cerr << "Error in recv " << std::endl;
		}
		else	{
			printf( "FROM THE VOID you see a msg-> %s", recvbuf); 
		}
	}
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
	sin.sin_addr.s_addr = (inet_addr(serverName));
	
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
