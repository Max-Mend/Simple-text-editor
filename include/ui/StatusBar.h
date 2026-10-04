#ifndef STATUSBAR_H
#define STATUSBAR_H

#pragma once
#include <QStatusBar>
#include <QLabel>

class StatusBar : public QStatusBar {
    Q_OBJECT
public:
    explicit StatusBar(QWidget *parent = nullptr);
    ~StatusBar() = default;

    void showMessage(const QString &message, int timeout = 0);
    void setLineColumnInfo(int line, int column);

private:
    QLabel *messageLabel;
    QLabel *lineColumnLabel;
};

#endif //STATUSBAR_H