#include "include/bind_thread.h"
#include "include/global_functions.h"

namespace msg
{
	bind_thread::bind_thread(SOCKET* server_socket, SOCKET* accept_socket, std::string server_address, int port, int buffer_size) : m_abort_flag(false),
		m_server_socket(server_socket), m_accept_socket(accept_socket), m_server_address(server_address), m_port(port) {}

	void bind_thread::set_abort_flag(bool flag) { m_abort_flag = flag; }

	void bind_thread::run()
	{
		while (!m_abort_flag)
		{
			WSADATA wsaData;
			int wsaerr;
			WORD wVersionRequested = MAKEWORD(2, 2);

			wsaerr = WSAStartup(wVersionRequested, &wsaData);
			if (wsaerr != 0)
			{
				msleep(3000);
				continue;
			}

			// client socket = server socket
			*m_server_socket = INVALID_SOCKET;
			*m_server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (*m_server_socket == INVALID_SOCKET)
			{
				msleep(3000);
				continue;
			}

			if (m_server_address == "localhost") // localhost
				m_server_address = "127.0.0.1";

			std::wstring w_address = wtos(m_server_address);

			// bind
			sockaddr_in service;
			service.sin_family = AF_INET;
			InetPton(AF_INET, _wcsdup(w_address.c_str()), &service.sin_addr.s_addr);
			service.sin_port = htons(m_port);
			if (bind(*m_server_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
			{
				closesocket(*m_server_socket);
				msleep(3000);
				continue;
			}

			if (listen(*m_server_socket, 1) == SOCKET_ERROR)
			{
				msleep(3000);
				continue;
			}

			*m_accept_socket = accept(*m_server_socket, NULL, NULL);
			if (*m_accept_socket == INVALID_SOCKET)
			{
				msleep(3000);
				continue;
			}
			else
			{
				m_abort_flag = true;
				return;
			}
		}
	}
}