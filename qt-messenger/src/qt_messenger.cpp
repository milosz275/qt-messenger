#include "include/qt_messenger.h"
#include "include/key_press.h"
#include "include/receive_thread.h"
#include "include/connect_thread.h"
#include "include/bind_thread.h"
#include "include/global_functions.h"

// todo:
// audio
// accept and client socket lists
// password verification
// rsa message encryption
// auto dark mode
// autocalc timers, one argument for update frequency
// add emojis
// text control
// add global timer values

namespace msg
{
	qt_messenger::qt_messenger(QWidget* parent) : QWidget(parent), m_ui(new Ui::qt_messengerClass), m_message_queue(new QQueue<std::string>()), m_receive_thread(NULL), m_connect_thread(NULL),
		m_bind_thread(NULL), m_is_online(false), m_is_connecting(false), m_separator(": "), m_client_socket(NULL), m_accept_socket(NULL), m_timer_id(0),
		m_seconds_connected(0), m_miliseconds_counter(0)
	{
		m_ui->setupUi(this);
		//Qt::ColorScheme

		m_ui->address->setPlainText("localhost");
		m_ui->status->setPlainText("Offline");
		m_ui->port->setPlainText("56701");
		m_ui->host_or_connect->addItem("server", 0);
		m_ui->host_or_connect->addItem("client", 1);
		m_ui->time_connected->setDigitCount(8);
		m_ui->time_connected->display(QString("00:00:00"));
		m_ui->password->setStyleSheet("border: 1px solid lightgrey");
		m_ui->password->setEchoMode(QLineEdit::Password);
		m_ui->nickname->setStyleSheet("border: 1px solid lightgrey");

		key_press* insert_text_filter = new key_press(this);
		m_ui->insert_text->installEventFilter(insert_text_filter);
	}

	qt_messenger::~qt_messenger()
	{
		std::string disconnect_message("disconnect");
		encrypt(disconnect_message);
		if (m_ui->host_or_connect->currentIndex() == 0) // server
			int byteCount = send(*m_accept_socket, disconnect_message.c_str(), m_buffer_size, 0);
		else if (m_ui->host_or_connect->currentIndex() == 1) // client
			int byteCount = send(*m_client_socket, disconnect_message.c_str(), m_buffer_size, 0);

		killTimer(m_timer_id);

		if (m_receive_thread != NULL)
			m_receive_thread->quit();
		if (m_ui->host_or_connect->currentIndex() == 0)
			m_bind_thread->quit();
		if (m_ui->host_or_connect->currentIndex() == 1)
			m_connect_thread->quit();

		closesocket(*m_client_socket);
		closesocket(*m_accept_socket);

		WSACleanup();

		if (m_client_socket == NULL)
			delete m_client_socket;

		if (m_accept_socket == NULL)
			delete m_accept_socket;

		delete m_message_queue;

		delete m_ui;
	}

	// connect button
	void qt_messenger::on_connect_button_clicked(void)
	{
		if (m_is_connecting)
		{
			m_ui->status->setPlainText("Program is already connecting");
			return;
		}
		else if (m_is_online)
		{
			m_ui->status->setPlainText("Program is already online");
			return;
		}

		if (m_ui->nickname->text().toStdString().empty())
		{
			#ifdef WINDOWS
			m_username = QString::fromStdWString(_wgetenv(L"username"));
			
			#else // POSIX systems
			//char username[32];
			//cuserid(username);
			//m_username = QString::fromUtf8(username);
			//m_username = QString::fromStdString(getlogin());
			m_username = QString::fromStdString("Nice user");

			#endif 

			m_ui->nickname->setText(m_username);
		}
		else
			m_username = m_ui->nickname->text();

		m_client_socket = new SOCKET();
		m_accept_socket = new SOCKET();

		m_ui->status->setPlainText("Connecting...");
		if (m_ui->host_or_connect->currentIndex() == 0)
		{
			m_bind_thread = new bind_thread(this->m_client_socket, this->m_accept_socket, m_ui->address->toPlainText().toStdString(), m_ui->port->toPlainText().toInt(), m_buffer_size);
			m_bind_thread->start();
		}
		else if (m_ui->host_or_connect->currentIndex() == 1)
		{
			m_connect_thread = new connect_thread(this->m_client_socket, this->m_accept_socket, m_ui->address->toPlainText().toStdString(), m_ui->port->toPlainText().toInt(), m_buffer_size);
			m_connect_thread->start();
		}

		m_is_connecting = true;
		m_timer_id = startTimer(100);
	}

	// disconnect button
	void qt_messenger::on_disconnect_button_clicked(void)
	{
		if (m_is_connecting)
		{
			m_ui->status->setPlainText("Offline");
			if (m_ui->host_or_connect->currentIndex() == 0)
				m_bind_thread->quit();
			if (m_ui->host_or_connect->currentIndex() == 1)
				m_connect_thread->quit();
			m_is_connecting = false;
			return;
		}
		else if (!m_is_online)
		{
			m_ui->status->setPlainText("Program is already offline");
			return;
		}

		// actual disconnection when program was online
		m_seconds_connected = 0;
		m_ui->time_connected->display(QString("00:00:00"));

		std::string disconnect_message("disconnect");
		encrypt(disconnect_message);
		if (m_ui->host_or_connect->currentIndex() == 0) // server
			int byteCount = send(*m_accept_socket, disconnect_message.c_str(), m_buffer_size, 0);
		else if (m_ui->host_or_connect->currentIndex() == 1) // client
			int byteCount = send(*m_client_socket, disconnect_message.c_str(), m_buffer_size, 0);

		m_ui->status->setPlainText("Offline");

		if (m_receive_thread != NULL)
		{
			m_receive_thread->set_abort_flag(true);
			m_receive_thread->quit();
			m_receive_thread->wait();
		}

		closesocket(*m_client_socket);
		closesocket(*m_accept_socket);

		killTimer(m_timer_id);
		WSACleanup();
		m_ui->messages->clear();

		if (m_client_socket == NULL)
			delete m_client_socket;

		if (m_accept_socket == NULL)
			delete m_accept_socket;

		m_is_online = false;
		m_is_connecting = false;

		QMutex mutex;
		mutex.lock();
		while (!(m_message_queue->empty()))
			m_message_queue->dequeue();
		mutex.unlock();
	}

	// send button
	void qt_messenger::on_send_button_clicked(void)
	{
		if (!m_is_online)
		{
			m_ui->status->setPlainText("Program is offline");
			m_ui->insert_text->clear();
			return;
		}

		QMutex mutex;
		QString message = m_ui->insert_text->toPlainText();
		std::string s_message = m_username.toStdString() + m_separator.toStdString() + m_ui->insert_text->toPlainText().toStdString();
		if (!(message.isEmpty()) || !(message == ""))
		{
			mutex.lock();
			m_message_queue->enqueue(s_message);
			mutex.unlock();
		}
		else
			return;

		encrypt(s_message);

		int byteCount = 0;

		if (m_ui->host_or_connect->currentIndex() == 0) // server
			byteCount = send(*m_accept_socket, s_message.c_str(), m_buffer_size, 0);
		else if (m_ui->host_or_connect->currentIndex() == 1) // client
			byteCount = send(*m_client_socket, s_message.c_str(), m_buffer_size, 0);

		m_ui->insert_text->clear();

		if (byteCount <= 0)
			on_disconnect_button_clicked();
	}

	void qt_messenger::timerEvent(QTimerEvent* event)
	{
		if (m_is_online)
		{
			m_miliseconds_counter += 100;
			if (m_miliseconds_counter == 1000)
			{
				m_miliseconds_counter = 0;
				m_seconds_connected += 1;
			}
			m_ui->time_connected->display(QDateTime::fromSecsSinceEpoch(m_seconds_connected, Qt::UTC).toString("hh:mm:ss"));
		}

		std::string message("");
		QMutex mutex;
		mutex.lock();
		if (!(m_message_queue->empty()))
			message = m_message_queue->dequeue();
		mutex.unlock();

		if (message == "disconnect")
			on_disconnect_button_clicked();
		else if (message != "")
			m_ui->messages->appendPlainText(message.c_str());

		if (m_is_connecting)
		{
			if (m_ui->host_or_connect->currentIndex() == 0) // server
			{
				if (m_bind_thread != NULL && !(m_bind_thread->isRunning())) // bind thread
				{
					m_is_connecting = false;
					m_is_online = true;
					m_ui->status->setPlainText("Hosting on: " + m_ui->address->toPlainText());
					m_bind_thread->quit();

					m_receive_thread = new receive_thread(this->m_message_queue, m_accept_socket);
					m_receive_thread->start();
				}
			}
			else if (m_ui->host_or_connect->currentIndex() == 1) // client
			{
				if (m_connect_thread != NULL && !(m_connect_thread->isRunning())) // connect thread
				{
					m_is_connecting = false;
					m_is_online = true;
					m_ui->status->setPlainText("Connected to: " + m_ui->address->toPlainText());
					m_connect_thread->quit();

					m_receive_thread = new receive_thread(this->m_message_queue, m_client_socket);
					m_receive_thread->start();
				}
			}
		}
	}
}