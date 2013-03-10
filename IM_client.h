/*
 * IM Client
 *
 * Spring 2013
 * Victoria Wu
 *
 * DEBUG
 * -after getting file list msg, it stays after error messages. overwriting messages?
 *
 */

#include <cstdlib>	//rand, srand
#include <process.h>	//multi threading
#include <time.h>	//srand time
#include <winsock.h>
#include <fstream>	//ftp
#include <sstream>
#include <iostream>	//writing to file
#include <iostream>	//writing to file
#include <iomanip>	//setw, setfill for padded zeroes
#include <queue>
#include "MessageQs.h"	//queues to handle messages
#pragma comment(lib, "wsock32.lib")	//link winsock lib
#pragma comment(lib, "libcmt.lib")	//for process.h




class IM_Client {
	private: 
	
		std::string name;
		SOCKET s;
		int msgNum;
		MessageQs* listener;
		
		//Dealing with file transfer
		std::string fileName;	//what file are they downloading
		int msgNumFileTransfer;	
		
		//Waits for ack from server.
		void waitForAck();
		void connectsock(const char* serverName, const char* portNum);

		//Threading functions.
		static void startListening(void* listener);	//monitor for messages from server
		static void startFileDownload(void* data);			
	
	public:
		IM_Client(const char* serverName, const char* portNum);
		~IM_Client();

		void displayNotifications();
		void displayAcks();
		
		void menuDisplay();
		void signIn();
		void checkMessages();
		void sendMessage();
		void getFileNames();
		void downloadFile();
		void logOut();
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
	name = "NO NAME";	
	srand(time(NULL));	//seed random.
	msgNum = rand() % 1000 + 10000;

	listener = new MessageQs(s);	
	fileName = "empty";
	msgNumFileTransfer = 0;
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

	//Sending sign on message
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
	std::getline(std::cin, name );

	fileName = name;
	msgNumFileTransfer = msgNum;
	//Begin downloading thread...
	_beginthread(startFileDownload, 0, (void*)this);	 
	//race condition...!!!! 
	msgNum++;	
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
		memset(recvbuf, '\0', bufferLength);
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
	

			//SOMETHING DEALING WITH A FILE
			if(atoi(messageNum.c_str()) == listener->getCurrentFileMsgNum())	{
				printf("L-> Found FILE MSG, putting in ack" );
				listener->putFileChunk(meat);
			}
			else if(code.compare("ack") ==0 || code.compare("Error")==0)	{
				printf("L-> putting in ack");
				listener->putAck(meat);		
			}
			else if (code.find("From") ==0 )	{
				printf("L-> putting in from");
				listener->putMessage(meat);
			}
			//notifications from server
			else	{
				std::cout << "putting notification " << std:: endl << meat;
				listener->putNotification(meat);
			}

//			printf( "-> %s", recvbuf); 
//			std::cout << "-> " << std::endl << std::endl;
		}
	
	}

}

void IM_Client::startFileDownload(void* d)	{

	if(DEBUG)
		std::cout << "FROM:THREAD Starting file download " << std::endl;
	IM_Client* c = (IM_Client*)d;
	std::cout << c-> s<< std::endl;

	//Get appropriate data.
	int currentMsgNum = c->msgNumFileTransfer;	//race condition? guaranteeing that we get right msg num, before we increment in main thread 
	MessageQs* mq = c->listener;
	SOCKET sock = c->s;
	std::string file = c->fileName;
	
	
	//Notify message queue to be on the lookout for these file messages, route to right queue.
	mq->setCurrentFileMsgNum(currentMsgNum);	
	//Construct socket message.
	std::stringstream ss;
	ss << currentMsgNum << ";5;" << file; 
	std::string message = ss.str();

	int bytes_sent = send(sock, message.c_str(), strlen(message.c_str() ), 0);
	if(bytes_sent == SOCKET_ERROR)	{
		std::cerr << "error in sending file request" << std::endl;
	}
	
	if(DEBUG)	{
		std::cout << "Sent " << bytes_sent << "bytes. " << std::endl;	
		std::cout << "Message: " << message.c_str() << std::endl;
	}

	//File transfer time!!	
	int blockCount = 1;		//current block
	int chunkSize = 528;	//full chunk size, used to determine which is last chunk
	std::string chunk;
	std::string fileContents;	//wait until file completely gotten before writing to file
	bool prevChunkOK = true;
	std::string notification;
	std::string errPrefix("Error");	
	do	{
		//Get message from server using msgnum.
		chunk = mq->findFileMessage(currentMsgNum);
		if(chunk.compare("") == 0) 	{	//could not find ack
			std::cout << "Couldn't find file msg ack." << std::endl;
//			prevChunkOK = false;	
			//Resend request for chunk. NOPE. Can't do. Gets ngry and expects next block request. 
			//Need to use cond variable to WAIT for block from server...
		}	
		else	{
			
			//If there was an ack message, for an ERROR
			if(chunk.compare(0, errPrefix.size(), errPrefix)==0)	{
				std::cout << "ERROR in file transfer!" << std::endl;
				prevChunkOK = false;
				notification += chunk;
			}
			else	{
				std::cout << "FOUND file ack for block " << blockCount << std::endl;
				//be sure to STRIP HEADER!!!! 
				fileContents += chunk;
				blockCount++;
				//request next chunk
				std::stringstream ss;
				ss << currentMsgNum << ";6;" << std::setw(5) <<std::setfill('0') << blockCount;
				std::string message = ss.str();
				
				int bytes_sent = send(sock, message.c_str(), strlen(message.c_str() ), 0);
				if(bytes_sent == SOCKET_ERROR)	{
					std::cerr << "error in sending sign in msg" << std::endl;	
				}

				if(DEBUG)	{
					std::cout << "Sent " << bytes_sent << "bytes. " << std::endl;	
					std::cout << "Message: " << message.c_str() << std::endl;
				}
			}
		}
	}
	//while message is OK and file is not completely here yet.
	while (prevChunkOK && chunk.length() >= chunkSize);

	if(DEBUG)	{
		std::cout << "Jumped out of file transfer loop." << std::endl;
		std::cout << "PrevChunk status: " << prevChunkOK << std::endl;
		std::cout << "Chunk length == " << chunk.length() << std::endl;

	}
	if(!prevChunkOK)	{
		//put appropriate notification in queue, will be printed out by main thread
		notification += "Ack message not received, or error in downloading file.";	
	}	
	else	{
		notification = "File successfully downloaded.";
	
	}	
	std::fstream fout;
	std::cout << "Writing contents to file. " << std::endl;
	fout.open(file, std::ios::out);
	fout << fileContents;
	fout.close();
	//Reset file msgnumber.
	mq->setCurrentFileMsgNum(-1);
	mq->putNotification(notification);
}


