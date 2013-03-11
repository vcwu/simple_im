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
	fileMsgNumber= -1;
	fileMsgPresent = false;
	s = in;
	TESTER = "HELLO";
	InitializeConditionVariable(&fileMsgAvailable);
	InitializeCriticalSection(&critSec);
}
MessageQs::~MessageQs()	{}

void MessageQs::displayAcks()	{
	EnterCriticalSection(&critSec);
	if(acks.empty())	{
		std::cout << "No new acks."  <<std::endl;
	}
	while(!acks.empty())	{
		std::cout << acks.front() << std::endl << std::endl;
		acks.pop_front();
	}
	LeaveCriticalSection(&critSec);
}



void MessageQs::displayNotifications()	{
	EnterCriticalSection(&critSec);
	while(!notifications.empty())	{
		std::cout << notifications.front() << std::endl;
		notifications.pop_front();
	}
	//std::cout << "NO NEW NOTIFICATIONS" << std::endl;
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

	if(DEBUG)
		std::cout << "Enter crit section " << std::endl;
	EnterCriticalSection(&critSec);
	
	//Wait until there is actually an appropriate file message with msgNum
	int FIFTEEN_SECONDS = 15000;	//time in ms
	if(!fileMsgPresent)	{
		//std::cout << "no file msg available, going to sleep " <<std::endl;
		//If it waits for too long, just scrap it and say error in downloading file!!
		SleepConditionVariableCS(&fileMsgAvailable, &critSec, FIFTEEN_SECONDS);
	}

	std::string message;	
	//Waited too long, error with file download.
	if(!fileMsgPresent)	{
		message = "";
		std::string notification = "Error with file download. Aborting."; 
		putNotification(notification);
	}
	else	{
		message = "";
		//std::cout << "WOKE UP! There's a msg for file , msgnum "<< msgNum << std::endl;
		std::deque<std::string>::iterator it;
		for(it = fileAcks.begin(); it!= fileAcks.end(); )	{
			std::stringstream ss;
			ss<<msgNum;
			std::string s = *it;
			//If you find msg num, return its msg.
			if(s.find(ss.str()) != std::string::npos)	{
			//	std::cout << "FOUND ACK!! " << std::endl;
				message = s;	
				it = fileAcks.erase(it);		
			}
			else	{
				++it;
			}
		}
	}
	//Be sure to set fileMsgPresent to FALSE.
	fileMsgPresent = false;	
	LeaveCriticalSection(&critSec);
//	std::cout << "ACKOK is done." <<std::endl;
	
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
	//std::cout << "putFileChunk enter crit section! " <<std::endl;
	EnterCriticalSection(&critSec);
	fileMsgPresent = true;
	fileAcks.push_back(m);
	//std::cout << "Set file msgPresent to " << fileMsgPresent << std::endl;
	//std::cout << "Pushed back fileAcks msg : " << m << std::endl;
	LeaveCriticalSection(&critSec);
	//std::cout << "Leave crit section, wake up cond variable" << std::endl;
	WakeConditionVariable(&fileMsgAvailable);
	//std::cout << "DONE WITH PUT FILE CHUNK" << std::endl;
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
