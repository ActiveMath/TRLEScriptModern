#pragma once
//#include <memory>
#include <forward_list>
#include <stack>
#include <string>

namespace TRLEScript
{
	enum class MessageTypes
	{
		Warning,
		Error
	};

	struct Message
	{
		MessageTypes type;
		std::string text;
	};

	class Notifier
	{
	public:

		virtual std::stack<Message> GetMessageStack() const;
		virtual std::string GetLastMessage();
		//virtual std::shared_ptr<std::string> GetLastMessage() const = 0;

		virtual std::stack<Message> GetWarningStack() const;
		virtual std::string GetLastWarning();
		//virtual std::shared_ptr<std::string> GetLastWarning() const = 0;

		virtual std::stack<Message> GetErrorStack() const;
		virtual std::string GetLastError();

		//virtual void AddNewMessage(MessageTypes type, std::string text);
		void Reset();

		//static std::forward_list<Message> messages;
		//virtual std::shared_ptr<std::string> GetLastError() const = 0;

		Notifier();
		virtual ~Notifier();

	protected:
		//std::stack<Message> messages;

		virtual void AddNewMessage(MessageTypes type, std::string text);

	private:
		std::forward_list<Message> messages;
	};
}
