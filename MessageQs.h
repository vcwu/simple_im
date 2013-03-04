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
		CONDITION_VARIABLE ackStatus;
		std::queue<std::string> notifications;
		std::queue<std::string> messages;
		std::queue<std::string> acks;
	public:
		MessageQs();
		~MessageQs();
		
		void displayNotifications();	//prints out notifications.

		void waitForAck(int msgNum);
		void putNotification(std::string m);
		void putMessage(std::string m);
		void putAck(std::string m);

		void getMessages();
};


MessageQs::MessageQs()	{
	InitializeCriticalSection(&critSec);
}
MessageQs::~MessageQs()	{}

void MessageQs::displayNotifications()	{
	EnterCriticalSection(&critSec);
	while(!notifications.empty())	{
		std::cout << notifications.front() << std::endl;
		notifications.pop();
	}
	std::cout << "NO NEW NOTIFICATIONS" << std::endl;
	LeaveCriticalSection(&critSec);
}

void MessageQs::waitForAck(int msgNum)	{
	EnterCriticalSection(&critSec);
	while(acks.empty())	{
		SleepConditionVariableCS(&ackStatus, &critSec, INFINITE);
	}	

	//acks queue now has something - check msg num
	LeaveCriticalSection(&critSec);
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
	LeaveCriticalSection(&critSec);	
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
