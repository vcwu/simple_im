/*
 * im_client
 * CS423 client server applications
 * victoria wu
 *
 * A collection of helper methods, and shared data ie buddy log, messages from
 * other peers.
 */

#ifndef IM_CLIENT_H
#define IM_CLIENT_H

#include <windows.h>	//crit sec, cond var
#include <sstream>	//for sending msgs  - change??
#include <string>
#include <map>
#include <vector>
#include "MySock.h"
class Im_client	{

	private:
		typedef std::pair< std::string, std::string> UsrInfo;
		typedef std::map< std::string, UsrInfo > BuddyLog;

		static const char DELIMETER = '#';

		//MySockets
//		MySock peerListener;
		SOCKET peerListener;
		MySock serverListener;
		
	
		//file download stuff
		//can only handle one file transfer at at time
		UsrInfo requestDownload;	//client side
		std::vector<u_short> fileStuff;		//server side

		std::string userName;
		BuddyLog log;

		//for a clean shutdown.
		boolean closingSocketTime;
		boolean readyToShutdown;
		CRITICAL_SECTION critSec;
		CONDITION_VARIABLE threadsDown;

		//MySock is NOT working for a passive socket.
		//This is a crude fix, taken from Comer's code.
		SOCKET passiveSock(std::string service, std::string transport, int qlen);
	public:
		Im_client();
		~Im_client();

		enum E_MSG_TYPE	{
			LOG_ON = 1,	
			SEND_MSG,
			STATUS_CHK,
			USR_UPDATE,
			FILE_LIST,
			FILE_DL
		};

		//threads functions yay
		static void listenToServer(void* me);
		static void listenToPeers(void* me);
		static void requestFile(void* me);
		static void sendFile(void* me);
		//Backend stuff.
		void displayMenu();
		void startup(int backlog, std::string serverName, std::string
				portNum, std::string listeningPort);
		void logOn(std::string name, std::string listeningPort);
		void parseServerMsg(std::string littleMsg);
		std::string findFiles();
		
		void shutdown();
		bool sendToBuddy(MySock &who, std::string buddy, std::string msg);

		//User functions.
		void sendMessage();
		void getFileNames();
		void downloadFile();
		
		//From other peers to me.
		

};

#endif
