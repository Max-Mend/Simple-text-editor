#include "TerminalWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QKeyEvent>

TerminalWidget::TerminalWidget(QWidget *parent) 
    : QWidget(parent), historyIndex(0) {
    setupUI();
    setupProcess();
}

TerminalWidget::~TerminalWidget() {
    if (process && process->state() != QProcess::NotRunning) {
        process->kill();
        process->waitForFinished();
    }
}

void TerminalWidget::setWorkingDirectory(const QString &dir) {
    currentDir = dir;
    if (process) {
        process->setWorkingDirectory(dir);
    }
    updatePrompt();
}

QString TerminalWidget::getWorkingDirectory() const {
    return currentDir;
}

void TerminalWidget::clear() {
    outputText->clear();
}

void TerminalWidget::onCommandEntered() {
    QString command = inputLine->text().trimmed();
    if (command.isEmpty()) return;

    commandHistory.append(command);
    historyIndex = commandHistory.size();

    appendOutput(currentPrompt + command, palette().color(QPalette::Text));

    if (command == "clear") {
        clear();
        inputLine->clear();
        return;
    }

    if (command.startsWith("cd ")) {
        QString newDir = command.mid(3).trimmed();
        changeDirectory(newDir);
        inputLine->clear();
        return;
    }

    executeCommand(command);
    inputLine->clear();
}

void TerminalWidget::onProcessOutput() {
    QString output = QString::fromUtf8(process->readAllStandardOutput());
    appendOutput(output, palette().color(QPalette::Text));
}

void TerminalWidget::onProcessError() {
    QString error = QString::fromUtf8(process->readAllStandardError());
    appendOutput(error, QColor(170, 0, 0));
}

void TerminalWidget::onProcessFinished(int exitCode, QProcess::ExitStatus status) {
    if (status == QProcess::CrashExit) {
        appendOutput("Process crashed!", QColor(170, 0, 0));
    } else if (exitCode != 0) {
        appendOutput(QString("Process exited with code %1").arg(exitCode),
                    QColor(170, 85, 0));
    }
    updatePrompt();
}

void TerminalWidget::onUpPressed() {
    if (historyIndex > 0) {
        historyIndex--;
        inputLine->setText(commandHistory[historyIndex]);
    }
}

void TerminalWidget::onDownPressed() {
    if (historyIndex < commandHistory.size() - 1) {
        historyIndex++;
        inputLine->setText(commandHistory[historyIndex]);
    } else {
        historyIndex = commandHistory.size();
        inputLine->clear();
    }
}

void TerminalWidget::onKillProcess() {
    if (process && process->state() != QProcess::NotRunning) {
        process->kill();
        appendOutput("^C", QColor(170, 0, 0));
    }
}

void TerminalWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(2, 2, 2, 2);
    mainLayout->setSpacing(2);

    QWidget *headerWidget = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(5, 2, 5, 2);

    QLabel *titleLabel = new QLabel("Terminal", headerWidget);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    mainLayout->addWidget(headerWidget);

    outputText = new QTextEdit(this);
    outputText->setReadOnly(true);
    QFont monoFont("Monospace", 9);
    monoFont.setStyleHint(QFont::TypeWriter);
    outputText->setFont(monoFont);
    mainLayout->addWidget(outputText);

    QWidget *inputWidget = new QWidget(this);
    QHBoxLayout *inputLayout = new QHBoxLayout(inputWidget);
    inputLayout->setContentsMargins(5, 2, 5, 2);

    promptLabel = new QLabel("$ ", inputWidget);
    QFont promptFont("Monospace", 9);
    promptFont.setBold(true);
    promptFont.setStyleHint(QFont::TypeWriter);
    promptLabel->setFont(promptFont);

    inputLine = new QLineEdit(inputWidget);
    inputLine->setFont(monoFont);
    connect(inputLine, &QLineEdit::returnPressed, this, &TerminalWidget::onCommandEntered);
    inputLine->installEventFilter(this);

    inputLayout->addWidget(promptLabel);
    inputLayout->addWidget(inputLine);

    mainLayout->addWidget(inputWidget);

    inputLine->setFocus();
}

void TerminalWidget::setupProcess() {
    process = new QProcess(this);
    currentDir = QDir::homePath();
    process->setWorkingDirectory(currentDir);

    connect(process, &QProcess::readyReadStandardOutput, this, &TerminalWidget::onProcessOutput);
    connect(process, &QProcess::readyReadStandardError, this, &TerminalWidget::onProcessError);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &TerminalWidget::onProcessFinished);

    updatePrompt();
}

void TerminalWidget::executeCommand(const QString &command) {
#ifdef Q_OS_WIN
    process->start("cmd.exe", QStringList() << "/c" << command);
#else
    process->start("sh", QStringList() << "-c" << command);
#endif
}

void TerminalWidget::changeDirectory(const QString &dir) {
    QDir newDir(currentDir);

    if (dir == "~") {
        currentDir = QDir::homePath();
    } else if (dir.startsWith("/")) {
        currentDir = dir;
    } else if (newDir.cd(dir)) {
        currentDir = newDir.absolutePath();
    } else {
        appendOutput("cd: no such directory: " + dir, QColor(170, 0, 0));
        return;
    }

    process->setWorkingDirectory(currentDir);
    updatePrompt();
}

void TerminalWidget::updatePrompt() {
    QString user = qgetenv("USER");
    if (user.isEmpty()) user = qgetenv("USERNAME");
    if (user.isEmpty()) user = "user";

    QString shortPath = currentDir;
    QString home = QDir::homePath();
    if (shortPath.startsWith(home)) {
        shortPath.replace(0, home.length(), "~");
    }

    currentPrompt = QString("%1@%2 $ ").arg(user).arg(shortPath);
    promptLabel->setText(currentPrompt);
}

void TerminalWidget::appendOutput(const QString &text, const QColor &color) {
    QTextCursor cursor = outputText->textCursor();
    cursor.movePosition(QTextCursor::End);
    
    QTextCharFormat format;
    format.setForeground(color);
    cursor.setCharFormat(format);
    cursor.insertText(text + "\n");
    
    outputText->setTextCursor(cursor);
    outputText->ensureCursorVisible();
}

bool TerminalWidget::eventFilter(QObject *obj, QEvent *event) {
    if (obj == inputLine && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Up) {
            onUpPressed();
            return true;
        } else if (keyEvent->key() == Qt::Key_Down) {
            onDownPressed();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}