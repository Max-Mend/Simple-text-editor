#ifndef TERMINALWIDGET_H
#define TERMINALWIDGET_H

#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QProcess>
#include <QLabel>
#include <QString>
#include <QStringList>

class TerminalWidget : public QWidget {
    Q_OBJECT

public:
    explicit TerminalWidget(QWidget *parent = nullptr);
    ~TerminalWidget();

    void setWorkingDirectory(const QString &dir);
    QString getWorkingDirectory() const;

public slots:
    void clear();

private slots:
    void onCommandEntered();
    void onProcessOutput();
    void onProcessError();
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);
    void onUpPressed();
    void onDownPressed();
    void onKillProcess();

private:
    void setupUI();
    void setupProcess();
    void executeCommand(const QString &command);
    void changeDirectory(const QString &dir);
    void updatePrompt();
    void appendOutput(const QString &text, const QColor &color);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QTextEdit *outputText;
    QLineEdit *inputLine;
    QLabel *promptLabel;
    QProcess *process;
    QString currentDir;
    QString currentPrompt;
    QStringList commandHistory;
    int historyIndex;
};

#endif // TERMINALWIDGET