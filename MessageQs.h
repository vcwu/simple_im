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
		std::queue<std::string> notifications;
		std::queue<std::string> messages;
		std::queue<std::string> acks;
	public:
		MessageQs();
		~MessageQs();

};

MessageQs::MessageQs()	{}
MessageQs::~MessageQs()	{}
