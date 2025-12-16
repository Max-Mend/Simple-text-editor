#include "StatusBar.h"
#include <QTimer>

StatusBar::StatusBar(QWidget *parent) : QStatusBar(parent) {
    // Create permanent widgets for the status bar
    messageLabel = new QLabel(this);
    messageLabel->setText("Ready");
    addWidget(messageLabel, 1); // Stretch factor 1 to take available space

    lineColumnLabel = new QLabel(this);
    lineColumnLabel->setText("Ln 1, Col 1");
    lineColumnLabel->setMinimumWidth(100);
    addPermanentWidget(lineColumnLabel);
}

void StatusBar::showMessage(const QString &message, int timeout) {
    messageLabel->setText(message);

    if (timeout > 0) {
        QTimer::singleShot(timeout, this, [this]() {
            messageLabel->setText("Ready");
        });
    }
}

void StatusBar::setLineColumnInfo(int line, int column) {
    lineColumnLabel->setText(QString::asprintf("Ln %d, Col %d", line, column));
}