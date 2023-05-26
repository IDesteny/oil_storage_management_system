#ifndef __MESSAGE_CONNECTION_HPP__
#define __MESSAGE_CONNECTION_HPP__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>

#include "connection_if.hpp"

class message_connection : public connection_if
{
private:
	struct message_buffer
	{
		long type;
		char buffer[1];
	};

	struct message_handle
	{
		int server_message_handle;
		int client_message_handle;
	};
	
	message_handle msg_handle { -1 };
	
	bool message_read(char *buffer, size_t size) noexcept
	{
		auto smart_buffer = std::vector<char>(sizeof(message_buffer) + size);
		auto msg_buffer = reinterpret_cast<message_buffer *>(smart_buffer.data());

		msg_buffer->type = 1;
		if (msgrcv(msg_handle.client_message_handle, msg_buffer, size, 1, 0) == -1)
		{
			return true;
		}

		std::memcpy(buffer, msg_buffer->buffer, size);
		return false;
	}

	bool message_write(const char *buffer, size_t size) noexcept
	{
		auto smart_buffer = std::vector<char>(sizeof(message_buffer) + size);
		auto msg_buffer = reinterpret_cast<message_buffer *>(smart_buffer.data());

		msg_buffer->type = 1;
		std::memcpy(msg_buffer->buffer, buffer, size);

		return msgsnd(msg_handle.server_message_handle, msg_buffer, size, 0) == -1;
	}

public:
	message_connection(status &init_status, int session_id = std::numeric_limits<int>::max()) noexcept
	{
	#ifdef _IS_SERVER_
		static const auto server_message_key = 1;
		static const auto client_message_key = 2;
		
		if ((msg_handle.server_message_handle = msgget(session_id >> server_message_key, IPC_CREAT | 400)) == -1)
		{
			init_status = status::failed_initialization;
			return;
		}

		if ((msg_handle.client_message_handle = msgget(session_id >> client_message_key, IPC_CREAT | 400)) == -1)
		{
			init_status = status::failed_initialization;
			return;
		}
	#else
		static const auto server_message_key = 2;
		static const auto client_message_key = 1;
		
		if ((msg_handle.server_message_handle = msgget(session_id >> server_message_key, 400)) == -1)
		{
			init_status = status::failed_initialization;
			return;
		}

		if ((msg_handle.client_message_handle = msgget(session_id >> client_message_key, 400)) == -1)
		{
			init_status = status::failed_initialization;
			return;
		}
	#endif // !_IS_SERVER_
	
		init_status = status::success;
	}

	status read(std::string &message) override
	{
		size_t message_length;
		if (message_read(reinterpret_cast<char *>(&message_length), sizeof(message_length)))
		{
			return status::read_error;
		}

		message.resize(message_length);
		if (message_read(message.data(), message_length))
		{
			return status::read_error;
		}

		return status::success;
	}

	status write(std::string_view message) override
	{
		size_t message_length = message.length();
		if (message_write(reinterpret_cast<const char *>(&message_length), sizeof(message_length)))
		{
			return status::write_error;
		}

		if (message_write(message.data(), message_length))
		{
			return status::write_error;
		}

		return status::success;
	}

	~message_connection() noexcept
	{
	#ifdef _IS_SERVER_
		if (msg_handle.client_message_handle != -1)
		{
			msgctl(msg_handle.client_message_handle, IPC_RMID, 0);
		}
		
		if (msg_handle.server_message_handle != -1)
		{
			msgctl(msg_handle.server_message_handle, IPC_RMID, 0);
		}
	#endif // !_IS_SERVER_
	}
};

#endif // !__MESSAGE_CONNECTION_HPP__
