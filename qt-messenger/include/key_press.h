#ifndef KEYPRESS_H
#define KEYPRESS_H

#include <qobject.h>
#include <qevent.h>
#include "include/qt_messenger.h"

namespace msg
{
	class key_press : public QObject
	{
		Q_OBJECT

	public:
		// creates a key filter object
		key_press(qt_messenger* messenger);

	private:
		// pointer to the widget object
		qt_messenger* m_messenger;
		// overloaded filter method checking pressed key
		bool eventFilter(QObject* obj, QEvent* event);
	};
}

#endif