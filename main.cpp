#include <QApplication>
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.setWindowTitle("Chora Spatium");
    mainWindow.setWindowIcon(QIcon("../assets/logo.svg"));
    mainWindow.resize(1200, 800);
    mainWindow.show();

    return QApplication::exec();
}