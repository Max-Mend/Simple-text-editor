#include "MenuBar.h"
#include <QObject>

MenuBar::MenuBar(QMainWindow *parent, QTabWidget *tabs, QTreeView *tree,
                 QStatusBar *statusBar, QFileSystemModel *model)
    : mainWindow(parent), tabWidget(tabs), treeView(tree),
      statusBar(statusBar), fileSystemModel(model)
{
    menuBar = parent->menuBar();

    QFont fontMenu("JetBrains Mono", 11);
    fontMenu.setFixedPitch(true);
    menuBar->setFont(fontMenu);

    setupFileMenu();
    setupEditMenu();
    setupPluginsMenu();
    setupSettingsMenu();
    setupHelpMenu();
}

MenuBar::~MenuBar() = default;

void MenuBar::setupFileMenu() {
    QMenu *fileMenu = menuBar->addMenu("&File");

    QAction *newFile = fileMenu->addAction("&New File");
    QObject::connect(newFile, &QAction::triggered, this, &MenuBar::onNewFile);

    QAction *openFileAction = fileMenu->addAction("&Open File");
    QObject::connect(openFileAction, &QAction::triggered, this, &MenuBar::onOpenFile);

    QAction *openFolderAction = fileMenu->addAction("&Open Folder");
    QObject::connect(openFolderAction, &QAction::triggered, this, &MenuBar::onOpenFolder);

    fileMenu->addSeparator();

    QAction *exitAction = fileMenu->addAction("E&xit");
    QObject::connect(exitAction, &QAction::triggered, &QApplication::quit);
}

void MenuBar::setupEditMenu() {
    QMenu *editMenu = menuBar->addMenu("&Edit");

    editMenu->addAction("Run");
    editMenu->addSeparator();

    QAction *saveFileAction = editMenu->addAction("&Save File");
    QObject::connect(saveFileAction, &QAction::triggered, this, &MenuBar::onSaveFile);

    QAction *saveAsAction = editMenu->addAction("Save &As...");
    QObject::connect(saveAsAction, &QAction::triggered, this, &MenuBar::onSaveFileAs);
}

void MenuBar::setupPluginsMenu() {
    QMenu *pluginsMenu = menuBar->addMenu("&Plugins");
    pluginsMenu->addAction("&Create plugin");

    QAction *openPlugin = pluginsMenu->addAction("&Open plugin");
    QObject::connect(openPlugin, &QAction::triggered, this, &MenuBar::onOpenPlugin);
}

void MenuBar::setupSettingsMenu() {
    QAction *preferencesAction = menuBar->addAction("&Settings");
    QObject::connect(preferencesAction, &QAction::triggered, this, &MenuBar::onOpenSettings);
}

void MenuBar::setupHelpMenu() {
    QMenu *helpMenu = menuBar->addMenu("&Info");
    QAction *aboutAction = helpMenu->addAction("&About");
    QObject::connect(aboutAction, &QAction::triggered, this, &MenuBar::onShowAbout);
}

void MenuBar::onNewFile() {
    emit newFileRequested();
}

void MenuBar::onOpenFile() {
    QString fileName = QFileDialog::getOpenFileName(mainWindow, "Open File", "", "All Files (*.*)");
    if (!fileName.isEmpty()) {
        emit fileOpenRequested(fileName);
    }
}

void MenuBar::onOpenFolder() {
    QString folderName = QFileDialog::getExistingDirectory(mainWindow, "Open Folder", "");
    if (!folderName.isEmpty()) {
        fileSystemModel->setRootPath(folderName);
        treeView->setRootIndex(fileSystemModel->index(folderName));
        statusBar->showMessage("Opened folder: " + folderName, 5000);
    }
}

void MenuBar::onSaveFile() {
    emit saveFileRequested(false);
}

void MenuBar::onSaveFileAs() {
    emit saveFileRequested(true);
}

void MenuBar::onOpenPlugin() {
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open Plugin", "", "*.py");
    if (!fileName.isEmpty()) {
        qDebug() << "Opened plugin:" << fileName;
        statusBar->showMessage("Plugin functionality not yet implemented", 5000);
    }
}

void MenuBar::onOpenSettings() {
    emit settingsRequested();
}

void MenuBar::onShowAbout() {
    emit aboutRequested();
}