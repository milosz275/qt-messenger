#include "include/receive_thread.h"
#include "include/global_functions.h"

namespace msg
{
	receive_thread::receive_thread(QQueue<std::string>* queue, SOCKET* receive_socket) : m_que(queue), m_abort_flag(false), m_receive_socket(receive_socket), m_sleep_time(100) {}

	void receive_thread::set_abort_flag(bool flag) { m_abort_flag = flag; }

	void receive_thread::run()
	{
		while (!m_abort_flag)
		{
			char buffer[8192];
			int byteCount = recv(*m_receive_socket, buffer, 8192, 0);
			if (byteCount > 0)
			{
				std::string message(buffer);
				decrypt(message);
				m_que->enqueue(message);
			}
			msleep(m_sleep_time);
		}
	}
}