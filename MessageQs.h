/*
 * Message Queues
 *
 * Spring 2013
 * VIctoria Wu
 */

#include <Windows.h>	//sleep fn
#include <queue>
#include <string>
#include <iostream>
#include "im_message.h"
class MessageQs	{
	private:
		CRITICAL_SECTION critSec;	
		CONDITION_VARIABLE fileMsgAvailable;
		volatile bool fileMsgPresent;
		int fileMsgNumber;
		std::deque<std::string> notifications;	//Stuff client didn't request.
		std::deque<std::string> messages;	//Messages from other users.
		std::deque<std::string> acks;		//Acks to stuff client requested.
		std::deque<std::string> fileAcks;	//Acks/errors specifically related to file. (only one at at time)
	public:
		MessageQs(SOCKET in);
		~MessageQs();
		
		SOCKET s;
		std::string TESTER;
		void displayNotifications();	//prints out notifications.

		void displayAcks();		//TESTING
		std::string findFileMessage(int msgNum);
		void putNotification(std::string m);
		void putMessage(std::string m);
		void putAck(std::string m);
		void putFileChunk(std::string m);
		int getCurrentFileMsgNum();
		void setCurrentFileMsgNum(int msgNum);	
		void getMessages();
};


MessageQs::MessageQs(SOCKET in)	{
	fileMsgPresent = false;
	s = in;
	TESTER = "HELLO";
	InitializeCriticalSection(&critSec);
}
MessageQs::~MessageQs()	{}

void MessageQs::displayAcks()	{
	EnterCriticalSection(&critSec);
	while(!acks.empty())	{
		std::cout << acks.front() << std::endl << std::endl;
		acks.pop_front();
	}
	std::cout << "NO NEW ACKS" << std::endl;
	LeaveCriticalSection(&critSec);
}



void MessageQs::displayNotifications()	{
	EnterCriticalSection(&critSec);
	while(!notifications.empty())	{
		std::cout << notifications.front() << std::endl;
		notifications.pop_front();
	}
	std::cout << "NO NEW NOTIFICATIONS" << std::endl;
	LeaveCriticalSection(&critSec);
}


/*
 * findMsg(int msgNum)
 * Looks through acks queue for message with this msgNum.
 */
/*
bool MessageQs::findMessage(int msgNum)	{
	EnterCriticalSection(&critSec);
	bool found = false;
	for each(std::string in acks)	{
		std::stringstream ss; 
		ss << msgNum;
		if(s.find(ss.str()) != std::string::npos)
			found = true;
	}	
	LeaveCriticalSection(&critSec);
	return found;
}
*/
/*
 * This is SPECIFICALLY for file transfer, since the server will always either repeat the file chunk or return an error message.
 * Searches through Ack queue to find ack corresponding to msgNum.
 * It should ALWAYS find an ack for the msgnum. If it doesn't, it will wait until one comes in. 
 * There should be a timeout, so it doesn't get stuck here forever.
 */
std::string MessageQs::findFileMessage(int msgNum)	{

	EnterCriticalSection(&critSec);
	
	//Wait until there is actually an appropriate file message with msgNum
	while(!fileMsgPresent)	{
		SleepConditionVariableCS(&fileMsgAvailable, &critSec, INFINITE);
	}
	std::string message = "";
	std::cout << "Searching for Ack Message with msgnum "<< msgNum << std::endl;
	std::deque<std::string>::iterator it;
	for(it = acks.begin(); it!= acks.end(); )	{
		std::stringstream ss;
		ss<<msgNum;
		std::string s = *it;
		//If you find msg num, return its msg.
		if(s.find(ss.str()) != std::string::npos)	{
			std::cout << "FOUND ACK!! " << std::endl;
			message = s;	
			it = acks.erase(it);		
		}
		else	{
			++it;
		}
	}

	//Be sure to set fileMsgPresent to FALSE.
	
	//Be sure to REMOVE the messages with the ack.
	/*
	std::deque<std::string>::iterator front = acks.begin();
	std::deque<std::string>::iterator back = acks.end();

	back = std::remove_if(front, back, matchMsgNum);	//problem with pred fn - how to get msgnum over
	acks.erase(back, acks.end());
	*/
	LeaveCriticalSection(&critSec);
	std::cout << "ACKOK is done." <<std::endl;
	
	return message;
}


void MessageQs::putNotification(std::string m)	{
	EnterCriticalSection(&critSec);
	notifications.push_back(m);
	LeaveCriticalSection(&critSec);	
}

void MessageQs::putMessage(std::string m)	{
	EnterCriticalSection(&critSec);
	messages.push_back(m);
	LeaveCriticalSection(&critSec);	
}

void MessageQs::putAck(std::string m)	{
	EnterCriticalSection(&critSec);
	acks.push_back(m);
//	std::cout << "AFter pushing ack, stack size: " << acks.size() <<std::endl;
	LeaveCriticalSection(&critSec);	
//	WakeAllConditionVariable(&ackStatus);
//	std::cout << "PUSHED ACK " << m << std:: endl;
}

int MessageQs::getCurrentFileMsgNum()	{
	return fileMsgNumber;
}

void MessageQs::setCurrentFileMsgNum(int msgNum)	{
	fileMsgNumber = msgNum;
}
void MessageQs::putFileChunk(std::string m)	{
	EnterCriticalSection(&critSec);
	fileMsgPresent = true;
	fileAcks.push_back(m);
	LeaveCriticalSection(&critSec);
	WakeConditionVariable(&fileMsgAvailable);
}
void MessageQs::getMessages()	{
	EnterCriticalSection(&critSec);
	if(messages.empty())	{
		std::cout << "No new messages." << std::endl;
	}
	else	{
		while(!messages.empty())	{
			std::cout << messages.front() << std::endl;
			messages.pop_front();
		}
	}
	LeaveCriticalSection(&critSec);	
}
