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

    QFont font("JetBrain Mono", 14);
    font.setFixedPitch(true);
    textEdit->setFont(font);

    mainWindow.setCentralWidget(textEdit);

    QFont fontmenu("JetBrain Mono", 11);
    font.setFixedPitch(true);
    QMenuBar *menuBar = mainWindow.menuBar();

    menuBar->setFont(fontmenu);

    QStatusBar *statusBar = new QStatusBar();
    mainWindow.setStatusBar(statusBar);

    QMenu *fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction("&New File");

    QAction *openFileAction = fileMenu->addAction("&Open File");
    QObject::connect(openFileAction, &QAction::triggered, [&](){
        QString fileName = QFileDialog::getOpenFileName(&mainWindow,
                            "Open File", "", "All Files (*.*)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                textEdit->setPlainText(file.readAll());
            }
        }
    });

    QAction *openFolderAction = fileMenu->addAction("&Open Folder");
    QObject::connect(openFolderAction, &QAction::triggered, [&](){
        QString folderName = QFileDialog::getExistingDirectory(&mainWindow,"Open Folder", "");
    });

    QMenu *editMenu = menuBar->addMenu("&Edit");
    editMenu->addAction("&Run");
    QAction *saveFileAction = editMenu->addAction("&Save File");
    QObject::connect(saveFileAction, &QAction::triggered, [&](){
        QString fileName = QFileDialog::getSaveFileName(&mainWindow,
                            "Save File", "", "All Files (*.*)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                file.write(textEdit->toPlainText().toUtf8());
                file.close();
                statusBar->showMessage("Saved file: " + fileName, 5000);
            } else {
                statusBar->showMessage("Failed to save file!", 5000);
            }
        }
    });

    editMenu->addAction("&Exit", QApplication::quit);

    QMenu *pluginsMenu = menuBar->addMenu("&Plugins");
    pluginsMenu->addAction("&Create plugin");
    QAction *openPlugin = pluginsMenu->addAction("&Open plugin");

    QObject::connect(openPlugin, &QAction::triggered, [&]() {
        QString fileName = QFileDialog::getOpenFileName(nullptr,
                            "Open Plugin", "", "Plugin Files (*.py)");
        if (!fileName.isEmpty()) {
            qDebug() << "Opened plugin:" << fileName;
        }
    });



    mainWindow.setWindowTitle("Simple text editor");
    mainWindow.resize(1200, 800);
    mainWindow.show();

    return QApplication::exec();
}