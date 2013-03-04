/*
 * Message Queues
 *
 * Spring 2013
 * VIctoria Wu
 */

#include <queue>
#include <string>

class MessageQs	{
	private:
		CRITICAL_SECTION critSec;	
		std::queue<std::string> notifications;
		std::queue<std::string> messages;
		std::queue<std::string> acks;
		std::queue<std::string> fileChunks;
	public:
		MessageQs();
		~MessageQs();
		
		void putNotification(std::string m);
		void putMessage(std::string m);
		void putAck(std::string m);
		void putFileChunk(std::string m);
};

MessageQs::MessageQs()	{
	InitializeCriticalSection(&critSec);
}
MessageQs::~MessageQs()	{}

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

void MessageQs::putFileChunk(std::string m)	{
	EnterCriticalSection(&critSec);
	fileChunks.push(m);
	LeaveCriticalSection(&critSec);	
}
