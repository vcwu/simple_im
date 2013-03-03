/*
 * IM Client
 *
 * Spring 2013
 * Victoria Wu
 */

#include <cstdlib>	//rand, srand
#include <process.h>	//multi threading
#include <time.h>	//srand time
#include <winsock.h>
#include <sstream>
#include <queue>
#include "MessageQs.h"	//queues to handle messages
#pragma comment(lib, "wsock32.lib")	//link winsock lib
#pragma comment(lib, "libcmt.lib")	//for process.h

class IM_Client {
	private: 
		std::string name;
		struct sharedinfo {
			SOCKET s;
			int msgNum;
			MessageQs listener;
		} shared;	//I don't know how to make this better D:
		void connectsock(const char* serverName, const char* portNum);
		static void startListening(void* listener);	//monitor for messages from server
	
	
	public:
		IM_Client(const char* serverName, const char* portNum);
		~IM_Client();


		void menuDisplay();
		void signIn();
		void logOut();
		void checkMessages();
		void sendMessage();
		void getFileNames();
		void downloadFile();
		void shutdown();		
};


IM_Client::IM_Client(const char* serverName, const char* portNum)	{

	//Link to winsock lib.
	WSADATA wsadata;
	if(WSAStartup(MAKEWORD(2,0), &wsadata) !=0)	{
		std::cerr << "WSAStartup failed.";
		exit(1);	
	}

	//Connect to a socket.
	connectsock(serverName, portNum);

	//Init other variables.
	srand(time(NULL));
	shared.msgNum = rand() % 1000 + 10000;

	name = "NO NAME";	
		
}

IM_Client::~IM_Client()	{}

/*
 * shutdown
 * releases socket.
 */
void IM_Client::shutdown()	{
	WSACleanup();
	closesocket(shared.s);
}

/*
 * connectsock - allocate and connect socket.
 * modified from Comer's Internetworking with TCP/IP Volume III, section 7.7
 *
 * @param serverName server IP address in dotted-decimal format
 * @param portNum	
 */
void IM_Client::connectsock(const char* serverName, const char* portNum)	{
	const char* transport = "udp";
	int  type;	//socket descriptor and socket type

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
	shared.s = socket(PF_INET, type, ppe->p_proto);
	if(shared.s == INVALID_SOCKET)	{
		std::cerr << "can't create socket";
		WSACleanup();
		exit(1);
		
	}

	//Connect socket.
	if(connect(shared.s, (struct sockaddr*) &sin, sizeof(sin))==SOCKET_ERROR) 	
	{
		std::cerr << "can't connect to " << serverName << " " << portNum;
		WSACleanup();
		exit(1);
	}

	std::cout << "Socket Creation and connection successful" <<std::endl;
}


void IM_Client::menuDisplay()	{
	std::cout << "What would you like to do?" << std::endl;
	std::cout << "\tCheck for messages (c)" << std::endl;
	std::cout << "\tSend a message (s)" << std::endl;
	std::cout << "\tGet a list of files to download (f) " << std::endl;
	std::cout << "\tDownlaod a file (d)" << std::endl;
	std::cout << "\tQuit (q)" << std::endl;
	std::cout << std::endl;
}

void IM_Client::signIn()	{	
	
	std::cout << "What is your IM name? " << std::endl;
	std::getline(std::cin, name);

	std::cout << "Signing in as " << name << std::endl;
	
	std::cout << "MSG NUM : " << shared.msgNum << std::endl;

	std::stringstream ss;
	ss << shared.msgNum << ";1;" << name;
	std::string message = ss.str();
	
	int bytes_sent = send(shared.s, message.c_str(), strlen(message.c_str() ), 0);
	if(bytes_sent == SOCKET_ERROR)	{
		std::cerr << "error in sending sign in msg" << std::endl;	
	}

	if(DEBUG)	{
		std::cout << "Sent " << bytes_sent << "bytes. " << std::endl;	
		std::cout << "Message: " << message.c_str() << std::endl;
	}
	shared.msgNum++;
	//display confirm from server
	//
		
	const int bufferLength = 256;
	char recvbuf[bufferLength];
	if(recv(shared.s, recvbuf, bufferLength, 0) == SOCKET_ERROR )	{
		std::cerr << "error in recv " << std::endl;
	}
	else	{
		printf("-> %s -> \n", recvbuf);
	}

	//Start thread to monitor connection to server.
	_beginthread(startListening, 0, (void*) &shared);	

}
void IM_Client::sendMessage()	{
	//Get user input.
	std::string buddy;
	std::string message;
	std::cout << "Send to: " << std::endl;
	std::getline(std::cin, buddy);
	std::cout << "Message: " << std::endl;
	std::getline(std::cin, message);

	//Construct socket message.
	std::stringstream ss;
	ss << shared.msgNum << ";2;" << name << std::endl;
	ss << buddy << std::endl;
	ss << message;
	std::string udpMessage = ss.str();

	int bytes_sent = send(shared.s, udpMessage.c_str(), strlen(udpMessage.c_str() ), 0);
	if(bytes_sent == SOCKET_ERROR)	{
		std::cerr << "error in sending user message" << std::endl;
	}
	else	{
		std::cout << "SENT message: " << message << std::endl;
	}
	shared.msgNum++;
}

void IM_Client::checkMessages()	{

}

/*
 * getFileNames
 */
void IM_Client::getFileNames()	{

	//Construct socket message.
	std::stringstream ss;
	ss << shared.msgNum << ";4;filelist" << std::endl;
	std::string message = ss.str();

	int bytes_sent = send(shared.s, message.c_str(), strlen(message.c_str() ), 0);
	if(bytes_sent == SOCKET_ERROR)	{
		std::cerr << "error in sending user message" << std::endl;
	}
	shared.msgNum++;

}
void IM_Client::downloadFile()	{

}

void IM_Client::logOut()	{
	std::cout << "Logging off... " << std::endl;
	
	char num[5];
	itoa(shared.msgNum, num, 10);
	std::stringstream ss;

	ss << num << ";3;" << name;

	std::string message = ss.str();

	int bytes_sent = send(shared.s, message.c_str(), strlen(message.c_str()), 0);
	if(bytes_sent == SOCKET_ERROR)	{
		std::cerr << "Error in sending log off msg" << std::endl;
	}
	if(DEBUG)	{
		std::cout << "Sent: " << bytes_sent << "bytes." <<std::endl;
		std::cout << "Msg: " << message << std::endl;
	}
}

/*
 * startListening 
 * monitors and displays messages from server.
 *
 * @param listener 
 */
void IM_Client::startListening(void* listener)	{
	if(DEBUG)
		std::cout << "Listening for server messages..." <<std::endl;
	
	//SOCKET s = *( (SOCKET*)socketNum);
	//if(DEBUG)
		//std::cout << "Listening to socket " << s << std::endl;
	const int bufferLength = 256;
	char recvbuf[bufferLength];
	
	while(true)	{
		std::cerr << "Error in recv " << std::endl;
		
	/*	if(recv(shared.s, recvbuf, bufferLength, 0 ) == SOCKET_ERROR)	{
		}
		else	{
			printf( "-> %s", recvbuf); 
			std::cout << "-> " << std::endl << std::endl;
		}
	*/
	}

}


