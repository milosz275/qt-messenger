#include "include/key_press.h"

namespace msg
{
    key_press::key_press(qt_messenger* messenger) : m_messenger(messenger) {}

    bool key_press::eventFilter(QObject* object, QEvent* event)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent* key = static_cast<QKeyEvent*>(event);
            if ((key->key() == Qt::Key_Enter) || (key->key() == Qt::Key_Return))
                m_messenger->on_send_button_clicked();
            else if (key->key() == Qt::Key_Escape)
                m_messenger->m_ui->insert_text->clear();
            else if (key->key() == Qt::Key_Backspace)
                m_messenger->m_ui->insert_text->textCursor().deletePreviousChar();
            else if (m_messenger->m_ui->insert_text->toPlainText().size() == 8192); // todo: limit paste size, key_space
            else
                return QObject::eventFilter(object, event);
            return true;
        }
        else
            return QObject::eventFilter(object, event);
        return false;
    }
}