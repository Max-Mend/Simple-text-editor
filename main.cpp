#include <QApplication>
#include <QMainWindow>
#include <QtWidgets/QtWidgets>
#include <QPlainTextEdit>
#include <QMenuBar>
#include <QFont>
#include <QStatusBar>
#include <QFileDialog>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QFileDialog fileDialog;

    QMainWindow mainWindow;


    QPlainTextEdit *textEdit = new QPlainTextEdit();
    QTextBlock *textblock = new QTextBlock();
    QStatusBar *statusBar = new QStatusBar();

    QFont font("JetBrain Mono", 14);
    font.setFixedPitch(true);
    textEdit->setFont(font);

    mainWindow.setCentralWidget(textEdit);
    mainWindow.setStatusBar(statusBar);

    QFont fontmenu("JetBrain Mono", 11);
    font.setFixedPitch(true);
    QMenuBar *menuBar = mainWindow.menuBar();

    menuBar->setFont(fontmenu);

    QMenu *fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction("&New File");
    fileMenu->addAction("&Open File");
    fileMenu->addAction("&Open Folder");

    QMenu *editMenu = menuBar->addMenu("&Edit");
    editMenu->addAction("&Run");
    editMenu->addAction("&Exit", QApplication::quit);

    mainWindow.setWindowTitle("Simple Text Editor");
    mainWindow.resize(1200, 800);
    mainWindow.show();

    return QApplication::exec();
}