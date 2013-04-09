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
#include "MySock.h"
class Im_client	{

	private:
		typedef std::pair< std::string, std::string> UsrInfo;
		typedef std::map< std::string, UsrInfo > BuddyLog;

		static const char DELIMETER = '#';

		//MySockets
		MySock peerListener;
		MySock serverListener;

		std::string userName;
		BuddyLog log;

		//for a clean shutdown.
		boolean closingSocketTime;
		boolean readyToShutdown;
		CRITICAL_SECTION critSec;
		CONDITION_VARIABLE threadsDown;
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

		//listener fn for threads
		static void listenToServer(void* me);

	
		//Backend stuff.
		void displayMenu();
		void startup(int backlog, std::string serverName, std::string portNum);
		void logOn(std::string name);
		void parseServerMsg(std::string littleMsg);
		void shutdown();

		//User functions.
		void sendMessage();
		void getFileNames();
		void downloadFile();
		//
		bool sendToBuddy(std::string buddy, std::string msg);

};

#endif
