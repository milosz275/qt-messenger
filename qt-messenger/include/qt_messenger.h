#ifndef QT_MESSENGER_H
#define QT_MESSENGER_H

#include <QtWidgets/QWidget>
#include "ui_qt_messenger.h"

#include <qobject.h>
#include <qthread.h>
#include <qqueue.h>
#include <qdatetime.h>

#include <iostream>
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
    class receive_thread;
    class connect_thread;
    class bind_thread;

    class qt_messenger : public QWidget
    {
        Q_OBJECT

    public:
        qt_messenger(QWidget* parent = nullptr);
        ~qt_messenger();
    
    private slots:
        void on_send_button_clicked(void);
        void on_connect_button_clicked(void);
        void on_disconnect_button_clicked(void);

    private:
        // ui of the widget
        Ui::qt_messengerClass* m_ui;
        // timer for the timer event
        int m_timer_id;
        // flag indicating if the server/client is connected and online
        bool m_is_online;
        // flag indicating if the server/client is currently connecting to the the client/server
        bool m_is_connecting;
        //
        int m_miliseconds_counter;
        // time since binding/connecting
        int m_seconds_connected;
        // contains environmental variable: username of the server/client
        QString m_username;
        //QString errorString;
        // contains text separator between username and actual message (for example: "username" + ": " + "message")
        QString m_separator;
        // queue of messages to display in the widget
        QQueue<std::string>* m_message_queue;
        // pointer for server thread
        bind_thread* m_bind_thread;
        // pointer for client thread
        connect_thread* m_connect_thread;
        // pointer for messages receiving thread
        receive_thread* m_receive_thread;
        // server/client socket
        SOCKET* m_client_socket;
        // accept socket
        SOCKET* m_accept_socket;
        // size of the message buffer
        const int m_buffer_size = 8192;
        // overloaded widget updating method
        void timerEvent(QTimerEvent* event);

        friend class key_press;
        friend class connect_thread;
        friend class bind_thread;
    };

}

#endif // !QT_MESSENGER_H