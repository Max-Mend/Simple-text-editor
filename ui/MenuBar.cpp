#include "MenuBar.h"
#include <QObject>
#include <QSettings>

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
    newFile->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));
    QObject::connect(newFile, &QAction::triggered, this, &MenuBar::onNewFile);

    QAction *openFileAction = fileMenu->addAction("&Open File");
    openFileAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
    QObject::connect(openFileAction, &QAction::triggered, this, &MenuBar::onOpenFile);

    QAction *openFolderAction = fileMenu->addAction("&Open Folder");
    openFolderAction->setShortcut(QKeySequence(Qt::SHIFT | Qt::CTRL | Qt::Key_F));
    QObject::connect(openFolderAction, &QAction::triggered, this, &MenuBar::onOpenFolder);

    fileMenu->addSeparator();

    QAction *exitAction = fileMenu->addAction("E&xit");
    QObject::connect(exitAction, &QAction::triggered, &QApplication::quit);
}

void MenuBar::setupEditMenu() {
    QMenu *editMenu = menuBar->addMenu("&Edit");

    QAction *runAction = editMenu->addAction("Run");
    runAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));

    editMenu->addSeparator();

    QAction *saveFileAction = editMenu->addAction("&Save File");
    saveFileAction->setShortcut(QKeySequence::Save);
    QObject::connect(saveFileAction, &QAction::triggered, this, &MenuBar::onSaveFile);

    QAction *saveAsAction = editMenu->addAction("Save &As...");
    saveAsAction->setShortcut(QKeySequence::SaveAs);
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
    preferencesAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));
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
    QSettings settings("ChoraEditor", "Chora");
    QString lastFolder = settings.value("lastFolder", QDir::homePath()).toString();

    QString folderName = QFileDialog::getExistingDirectory(mainWindow, "Open Folder", lastFolder);
    if (!folderName.isEmpty()) {
        fileSystemModel->setRootPath(folderName);
        treeView->setRootIndex(fileSystemModel->index(folderName));
        statusBar->showMessage("Opened folder: " + folderName, 5000);
        settings.setValue("lastFolder", folderName);
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
        statusBar->showMessage("Plugin functionality not yet implemented", 5000);
    }
}

void MenuBar::onOpenSettings() {
    emit settingsRequested();
}

void MenuBar::onShowAbout() {
    emit aboutRequested();
}