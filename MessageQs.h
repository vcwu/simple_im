/*
 * Message Queues
 *
 * Spring 2013
 * VIctoria Wu
 */

#include <queue>
#include <string>
#include <iostream>
#include <sstream>
class MessageQs	{
	private:
		CRITICAL_SECTION critSec;	
//		CONDITION_VARIABLE ackStatus;
		std::queue<std::string> notifications;	//Stuff client didn't request.
		std::queue<std::string> messages;	//Messages from other users.
		std::queue<std::string> acks;		//Acks to stuff client requested.
		std::queue<std::string> file;		//used for ftp -> overkill?
	public:
		MessageQs(SOCKET in);
		~MessageQs();
		
		SOCKET s;
		std::string TESTER;
		void displayNotifications();	//prints out notifications.

		void displayAcks();		//TESTING
		bool waitForAck(int msgNum);
		bool checkAck(std::string message, int msgNum);
		void putNotification(std::string m);
		void putMessage(std::string m);
		void putAck(std::string m);
		void putFileChunk(std::string m);
		void getMessages();
};


MessageQs::MessageQs(SOCKET in)	{
	s = in;
	TESTER = "HELLO";
	InitializeCriticalSection(&critSec);
}
MessageQs::~MessageQs()	{}

void MessageQs::displayAcks()	{
	EnterCriticalSection(&critSec);
	while(!acks.empty())	{
		std::cout << acks.front() << std::endl;
		acks.pop();
	}
	std::cout << "NO NEW ACKS" << std::endl;
	LeaveCriticalSection(&critSec);
}

void MessageQs::putFileChunk(std::string m)	{
	EnterCriticalSection(&critSec);
	file.push(m);
	LeaveCriticalSection(&critSec);
}

void MessageQs::displayNotifications()	{
	EnterCriticalSection(&critSec);
	while(!notifications.empty())	{
		std::cout << notifications.front() << std::endl;
		notifications.pop();
	}
	std::cout << "NO NEW NOTIFICATIONS" << std::endl;
	LeaveCriticalSection(&critSec);
}

bool MessageQs::checkAck(std::string message, int msgNum)	{
	if(acks.empty())	
		return false;
	//do string stream, parsing again
	std::stringstream ss(message);		
	std::string code, messageNum, remainder;

	std::getline(ss, code, ';');
	std::getline(ss, messageNum, ';');
	std::getline(ss, remainder, ';');
	
	return atoi(messageNum.c_str())==msgNum;

}

bool MessageQs::waitForAck(int msgNum)	{
	std::cout << "MESSAGE Q: waiting for ack" << std::endl;
	bool ackOK = false;
	std::cout << "Ack Stack size: " << acks.size() << std::endl;

	//NEEDS SERIOUS WORK. DOESN"T WORK RIGHT NOW___
	//Blahsdfl;ajksdf
	//SleepConditionVariableCS(&ackStatus, &critSec, 5);
	
	//while(acks.empty())	{}	
	//ifacks queue now has something - check msg num
	EnterCriticalSection(&critSec);
	std::cout << "found something in ack!" << std::endl;
	if(!acks.empty())	{
		if(checkAck(acks.front(), msgNum))	{
			acks.pop();
			ackOK = true;
		}
	}
		
	LeaveCriticalSection(&critSec);
	std::cout << "ACKOK is done." <<std::endl;
	return ackOK;
}


void MessageQs::putNotification(std::string m)	{
	EnterCriticalSection(&critSec);
	notifications.push(m);
	LeaveCriticalSection(&critSec);	
}

void MessageQs::putMessage(std::string m)	{
	EnterCriticalSection(&critSec);
	messages.push(m);
	LeaveCriticalSection(&critSec);	
}

void MessageQs::putAck(std::string m)	{
	EnterCriticalSection(&critSec);
	acks.push(m);
//	std::cout << "AFter pushing ack, stack size: " << acks.size() <<std::endl;
	LeaveCriticalSection(&critSec);	
//	WakeAllConditionVariable(&ackStatus);
//	std::cout << "PUSHED ACK " << m << std:: endl;
}

void MessageQs::getMessages()	{
	EnterCriticalSection(&critSec);
	if(messages.empty())	{
		std::cout << "No new messages." << std::endl;
	}
	else	{
		while(!messages.empty())	{
			std::cout << messages.front() << std::endl;
			messages.pop();
		}
	}
	LeaveCriticalSection(&critSec);	
}
