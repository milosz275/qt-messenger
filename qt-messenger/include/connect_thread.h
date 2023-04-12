#ifndef CONNECTTHREAD_H
#define CONNECTTHREAD_H 

#include <qthread.h>
#include <QDebug>
#include <QMutex>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <atlbase.h>
#include <atlcom.h>
//#define _WIN32_WINNT 0x0501
#define _ATL_STATIC_LIB_IMPL
#define _ATL_DEBUG_INTERFACES
#pragma comment(lib, "Ws2_32.lib")

namespace msg
{
    class connect_thread : public QThread
    {
    public:
        // creates a thread containing pointer to client and accept socket
        connect_thread(SOCKET* client_socket, SOCKET* accept_socket, std::string server_address, int port, int buffer_size);
        //
        //~connect_thread();
        // overloaded run method
        void run();
        // sets the abort flag
        void set_abort_flag(bool flag);

    private:
        // flag to keep the main loop going
        bool m_abort_flag;
        // socket for connecting to the server
        SOCKET* m_client_socket;
        // socket for accepted server socket
        SOCKET* m_accept_socket;
        // address to bind
        std::string m_server_address;
        // port to bind
        int m_port;
        // size of the message buffer 
        static const int m_buffer_size = 8192;
    };
}

#endif