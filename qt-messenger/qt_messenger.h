#pragma once

#include <QtWidgets/QWidget>
#include "ui_qt_messenger.h"

class qt_messenger : public QWidget
{
    Q_OBJECT

public:
    qt_messenger(QWidget *parent = nullptr);
    ~qt_messenger();

private:
    Ui::qt_messengerClass ui;
};
