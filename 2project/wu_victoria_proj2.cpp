/*
 * Project 2
 * CS 423
 * victoria wu
 * spring 2013*
 *
 * Peer to Peer programming, connection oriented client/server
 * 
 * Main thread.
 * 	Creates socket for listening to peer requests and connecting to server.
 * 	Starts up thread to handle listening to server and peer requests.
 * 	Interacts with user. 
 
 */



#include "MySock.h"

#define WSVERS MAKEWORD(2,0)	//Win sock version.

//Compiled using Visual Studio's command prompt
#pragma comment(lib, "wsock32.lib")	//link winsock li
#pragma comment(lib, "libcmt.lib")	//for process.h

int main()	{

	//WSA STARTUP!!
	WSADATA wsadata;
//	MySock::startup(WSVERS, wsadata);	
	//Start listening for peer requests.
//	MySock peerListener("tcp");
	//Start listening for server requests.
	//Log on to server.
	//GUI loop
	

	//WSA CLEANUP!!
	//MySock::cleanup();
}
