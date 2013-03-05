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
#include <fstream>	//ftp
#include <sstream>
#include <queue>
#include "MessageQs.h"	//queues to handle messages
#pragma comment(lib, "wsock32.lib")	//link winsock lib
#pragma comment(lib, "libcmt.lib")	//for process.h

class IM_Client {
	private: 
	
		//used for file transfer.
		//not sure of more elegant way to do this T.T
		struct temp	{
			int num;	//msgNum for file transfer
			std::string fileName;
			MessageQs* mq;
		};

		std::string name;
		SOCKET s;
		int msgNum;
		MessageQs* listener;
		
		//Waits for ack from server.
		void waitForAck();
		void connectsock(const char* serverName, const char* portNum);
		static void startListening(void* listener);	//monitor for messages from server
		static void startFileDownload(void* data);			
	
	public:
		IM_Client(const char* serverName, const char* portNum);
		~IM_Client();

		void displayNotifications();
		void displayAcks();
		
		void menuDisplay();
		void signIn();
		void logOut();
		void checkMessages();
		void sendMessage();
		void getFileNames();
		void downloadFile();
		void shutdown();		
};


void IM_Client::waitForAck()	{
	if(!listener->waitForAck(msgNum))	{
		std::cout << "ACK msg# incorrect!! " << std::endl;
	}
}

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
	msgNum = rand() % 1000 + 10000;

	name = "NO NAME";	
	listener = new MessageQs(s);	
	listener->TESTER = "MY COPY";	
}

IM_Client::~IM_Client()	{
	delete listener;
}

/*
 * shutdown
 * releases socket.
 */
void IM_Client::shutdown()	{
	closesocket(s);
	WSACleanup();
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
	struct protoent *ppe;	// pointer to protocol information entr

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
}


void IM_Client::displayAcks()	{
	listener->displayAcks();
}
void IM_Client::displayNotifications()	{
	listener->displayNotifications();
}

void IM_Client::menuDisplay()	{
	std::cout << "What would you like to do?" << std::endl;
	std::cout << "\tCheck for messages (c)" << std::endl;
	std::cout << "\tSend a message (s)" << std::endl;
	std::cout << "\tGet a list of files to download (f) " << std::endl;
	std::cout << "\tDownload a file (d)" << std::endl;
	std::cout << "\tQuit (q)" << std::endl;
	std::cout << std::endl;
}

void IM_Client::signIn()	{	
	
	std::cout << "What is your IM name? " << std::endl;
	std::getline(std::cin, name);

	std::cout << "Signing in as " << name << std::endl;
	
	std::cout << "MSG NUM : " << msgNum << std::endl;

	std::stringstream ss;
	ss << msgNum << ";1;" << name;
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
	//display confirm from servr
		
	const int bufferLength = 256;
	char recvbuf[bufferLength];
	if(recv(s, recvbuf, bufferLength, 0) == SOCKET_ERROR )	{
		std::cerr << "error in recv " << std::endl;
	}
	else	{
		printf("-> %s -> \n", recvbuf);
	}

	//Start thread to monitor connection to server.
	_beginthread(startListening, 0, (void*) listener);	

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
	ss << msgNum << ";2;" << name << std::endl;
	ss << buddy << std::endl;
	ss << message;
	std::string udpMessage = ss.str();

	int bytes_sent = send(s, udpMessage.c_str(), strlen(udpMessage.c_str() ), 0);
	if(bytes_sent == SOCKET_ERROR)	{
		std::cerr << "error in sending user message" << std::endl;
	}
	else	{
		std::cout << "SENT message: " << udpMessage << std::endl;
	}

	//waitForAck();
	msgNum++;	

}

void IM_Client::checkMessages()	{
	std::cout << "CHECKING MESSAGES" << std::endl;
	listener->getMessages();
}

/*
 * getFileNames
 */
void IM_Client::getFileNames()	{

	//Construct socket message.
	std::stringstream ss;
	ss << msgNum << ";4;filelist" << std::endl;
	std::string message = ss.str();

	int bytes_sent = send(s, message.c_str(), strlen(message.c_str() ), 0);
	if(bytes_sent == SOCKET_ERROR)	{
		std::cerr << "error in sending user message" << std::endl;
	}
	
	if(DEBUG)	{
		std::cout << "Sent " << bytes_sent << "bytes. " << std::endl;	
		std::cout << "Message: " << message.c_str() << std::endl;
	}		

	//waitForAck();	
	msgNum++;

}
void IM_Client::downloadFile()	{
	//get filename
	std::string name;
	std::cout << "What file would you like to download? " << std::endl;
	std::cin >> name;

	struct temp *t = (temp*) malloc(sizeof (struct temp)); 
	t->num = msgNum;
	t->mq = listener;
	t->fileName= name;
	/*
	int* temp = new int[2];
	temp[0] = s;
	temp[1] = msgNum;
	*/
	
	std::cout << "Starting file download " << std::endl;
	//Begin downloading thread...
	_beginthread(startFileDownload, 0, (void*) t);	

	//Done with 	
}

void IM_Client::logOut()	{
	std::cout << "Logging off... " << std::endl;
	
	char num[5];
	itoa(msgNum, num, 10);
	std::stringstream ss;

	ss << num << ";3;" << name;

	std::string message = ss.str();

	int bytes_sent = send(s, message.c_str(), strlen(message.c_str()), 0);
	if(bytes_sent == SOCKET_ERROR)	{
		std::cerr << "Error in sending log off msg" << std::endl;
	}
	if(DEBUG)	{
		std::cout << "Sent: " << bytes_sent << "bytes." <<std::endl;
		std::cout << "Msg: " << message << std::endl;
	}
	//waitForAck();
}

/*
 * startListening 
 * monitors and displays messages from server.
 *
 * @param listener 
 */
void IM_Client::startListening(void* l)	{
	if(DEBUG)
		std::cout << "Listening for server messages..." <<std::endl;
	
	MessageQs* listener = ( (MessageQs*) l);
	const int bufferLength = 550;
	char recvbuf[bufferLength];
	
	while(true)	{
		
		if(recv(listener->s, recvbuf, bufferLength, 0 ) == SOCKET_ERROR)	{
			//not really share what to do here.			

		}
		else	{
			//Put data in appropriate queue.
			std::stringstream ss(recvbuf);		
			std::string meat(recvbuf);
			std::string code, messageNum, remainder;

			std::getline(ss, code, ';');
			std::getline(ss, messageNum, ';');
			std::getline(ss, remainder, ';');
	
/*
			if(code.compare("ack") ==0 || || code.compare("Error")==0)	{
				//if(atoi(messageNum.c_str()) != listener->msgNum){
				//	printf("Wrong ack num");
				//}
				printf("L-> putting in ack");
				listener->putAck(meat);		
			}
			else if (code.find("From") ==0 )	{
				printf("L-> putting in from");
				listener->putMessage(meat);
			}
			else if (code.compare("fil") ==0)	{
				printf("L->putting in files");
				listener->putFileChunk(meat);
			}
			//notifications from server
			else	{
				std::cout << "putting notification " << std:: endl << meat;

				listener->putNotification(meat);
			}
*/
			listener->putAck(meat);
//			printf( "-> %s", recvbuf); 
//			std::cout << "-> " << std::endl << std::endl;
		}
	
	}

}

void IM_Client::startFileDownload(void* d)	{

	if(DEBUG)
		std::cout << "FROM:THREAD Starting file download " << std::endl;
	//get socket, msgNum
	/*int* data = (int*) d;	
	SOCKET s = data[0];
	int msgNum = data[1];
	*/
	
	struct temp *t = (temp*) d;
	MessageQs* listener = t->mq;
	SOCKET s = listener->s;
	int msgNum = t->num;
	std::string file = t->fileName;
	//Construct socket message.
	std::stringstream ss;
	ss << msgNum << ";5;" << file << std::endl;
	std::string message = ss.str();

	int bytes_sent = send(s, message.c_str(), strlen(message.c_str() ), 0);
	if(bytes_sent == SOCKET_ERROR)	{
		std::cerr << "error in sending file request" << std::endl;
	}
	
	if(DEBUG)	{
		std::cout << "Sent " << bytes_sent << "bytes. " << std::endl;	
		std::cout << "Message: " << message.c_str() << std::endl;
	}		

	//Depending on ack, proceed or not
	//assume they ask for valid file.
	
	int block = 2;	//request next block of data
	int chunkSize = 512;	//full chunk size, used to determine last chunk
	
	std::fstream fout;
	fout.open(file, std::fstream::out);
	std::string chunk;	
	do 
	{
		chunk = listener->getFileChunk();	
		fout << chunk;
	}while(chunk.length() >= chunkSize);
	fout.close();
}

