#include "Notifier.h"

namespace TRLEScript
{

	Notifier::Notifier()
	{
	}

	std::stack<Message> Notifier::GetMessageStack() const
	{
		std::stack<Message> messageStack;

		for (auto message : messages)
		{
			messageStack.push(message);
		}

		return messageStack;
	}

	std::string Notifier::GetLastMessage()
	{
		if (messages.empty())
			return nullptr;

		std::string message = messages.front().text;
		return message;		//work may not
	}

	std::stack<Message> Notifier::GetWarningStack() const
	{
		std::stack<Message> messageStack;
		for (auto message : messages)
		{
			if (message.type == MessageTypes::Warning)
				messageStack.push(message);
		}

		return messageStack;
	}

	std::string Notifier::GetLastWarning()
	{
		if (messages.empty())
			return nullptr;

		for (auto message : messages)
		{
			if (message.type == MessageTypes::Warning)
			{
				std::string warning = message.text;
				return warning;			//may not
			}
		}

		return nullptr;
	}

	std::stack<Message> Notifier::GetErrorStack() const
	{
		std::stack<Message> messageStack;
		for (auto message : messages)
		{
			if (message.type == MessageTypes::Error)
				messageStack.push(message);
		}

		return messageStack;
	}

	std::string Notifier::GetLastError()
	{
		if (messages.empty())
			return nullptr;

		for (auto message : messages)
		{
			if (message.type == MessageTypes::Error)
			{
				std::string error = message.text;
				return error;
			}
		}

		return nullptr;
	}

	void Notifier::AddNewMessage(MessageTypes type, std::string text)
	{
		Message message;
		message.type = type;
		message.text = text;

		messages.push_front(message);
	}

	void Notifier::Reset()
	{
		messages.clear();
	}

	Notifier::~Notifier()
	{
	}
}