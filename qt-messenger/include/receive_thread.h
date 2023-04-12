#ifndef RECEIVETHREAD_H
#define RECEIVETHREAD_H

#include <qthread.h>
#include <qqueue.h>
#include <QDebug>
#include <QMutex>
#include <winsock2.h>

namespace msg
{
    class receive_thread : public QThread
    {
    public:
        // creates a thread for receiving messages from socket and appending them to the queue
        receive_thread(QQueue<std::string>* queue, SOCKET* receive_socket);
        //
        //~receive_thread();
        // overloaded run method
        void run();
        // sets the abort flag
        void set_abort_flag(bool flag);

    private:
        // pointer to the message queue
        QQueue<std::string>* m_que;
        // pointer to the messaging socket
        SOCKET* m_receive_socket;
        // flag to keep the main loop going
        bool m_abort_flag;
        // sleep time in miliseconds, has to be a divisor of 1000
        int m_sleep_time;
    };
}

#endif

