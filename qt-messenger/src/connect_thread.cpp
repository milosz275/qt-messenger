#include "include/connect_thread.h"
#include "include/global_functions.h"

namespace msg
{
	connect_thread::connect_thread(SOCKET* client_socket, SOCKET* accept_socket, std::string server_address, int port, int buffer_size) : m_abort_flag(false),
		m_client_socket(client_socket), m_accept_socket(accept_socket), m_server_address(server_address), m_port(port) {}

	void connect_thread::set_abort_flag(bool flag) { m_abort_flag = flag; }

	void connect_thread::run()
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

			*m_client_socket = INVALID_SOCKET;
			*m_client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (*m_client_socket == INVALID_SOCKET)
			{
				msleep(3000);
				continue;
			}

			// connection
			sockaddr_in clientService;
			clientService.sin_family = AF_INET;

			if (m_server_address == "localhost") // localhost
				m_server_address = "127.0.0.1";
			else
			{
				DWORD dwRetval;
				struct addrinfo* result = NULL;
				struct addrinfo hints;
				ZeroMemory(&hints, sizeof(hints));
				hints.ai_family = AF_UNSPEC;
				hints.ai_socktype = SOCK_STREAM;
				hints.ai_protocol = IPPROTO_TCP;
				dwRetval = getaddrinfo(m_server_address.c_str(), "0", &hints, &result);
				if (dwRetval != 0)
				{
					//std::cout << "getaddrinfo() failed with error: " << dwRetval;
					msleep(3000);
					continue;
				}
				else
				{
					std::cout << "getaddrinfo() succeeded" << std::endl;
				}
				struct sockaddr_in* sockaddr_ipv4;
				sockaddr_ipv4 = (struct sockaddr_in*)result->ai_addr;

				m_server_address = inet_ntoa(sockaddr_ipv4->sin_addr);
			}

			std::wstring w_address = wtos(m_server_address);

			InetPton(AF_INET, _wcsdup(w_address.c_str()), &clientService.sin_addr.s_addr);
			clientService.sin_port = htons(m_port);
			if (::connect(*m_client_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
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