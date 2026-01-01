#include "MainWindow.h"
#include "MenuBar.h"
#include "SettingsDialog.h"
#include "AboutDialog.h"
#include "StatusBar.h"
#include "TerminalWidget.h"
#include "../core/codeeditor.h"

#include <QFileSystemModel>
#include <QTreeView>
#include <QTabWidget>
#include <QSplitter>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QSettings>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    autoSaveEnabled = true;
    autoSaveInterval = 3;

    setupUI();
    setupConnections();
    restoreSettings();

    autoSaveTimer = new QTimer(this);
    autoSaveTimer->setSingleShot(true);
    connect(autoSaveTimer, &QTimer::timeout, this, &MainWindow::autoSaveCurrentFile);
}

MainWindow::~MainWindow() {
    saveSettings();
}

void MainWindow::setupUI() {
    fileSystemModel = new QFileSystemModel(this);
    fileSystemModel->setRootPath("");
    fileSystemModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);

    // Налаштування дерева файлів
    treeView = new QTreeView(this);
    treeView->setModel(fileSystemModel);
    treeView->setRootIndex(fileSystemModel->index(QDir::homePath()));

    treeView->setColumnHidden(1, true);
    treeView->setColumnHidden(2, true);
    treeView->setColumnHidden(3, true);
    treeView->setHeaderHidden(true);

    treeView->setMaximumWidth(300);
    treeView->setMinimumWidth(150);

    treeView->setAnimated(true);
    treeView->setIndentation(20);
    treeView->setSortingEnabled(true);

    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    tabWidget->setTabPosition(QTabWidget::North);

    terminal = new TerminalWidget(this);
    terminal->setWorkingDirectory(QDir::homePath());

    editorSplitter = new QSplitter(Qt::Vertical, this);
    editorSplitter->addWidget(tabWidget);
    editorSplitter->addWidget(terminal);
    editorSplitter->setStretchFactor(0, 3);
    editorSplitter->setStretchFactor(1, 1);

    mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(treeView);
    mainSplitter->addWidget(editorSplitter);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 4);
    mainSplitter->setChildrenCollapsible(false);

    setCentralWidget(mainSplitter);

    statusBar = new StatusBar(this);
    setStatusBar(statusBar);

    menuBar = new MenuBar(this, tabWidget, treeView, statusBar, fileSystemModel);

    editorFont = QFont("JetBrains Mono", 14);
    editorFont.setFixedPitch(true);
}

void MainWindow::setupConnections() {
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(treeView, &QTreeView::doubleClicked, this, &MainWindow::onTreeViewDoubleClicked);

    connect(menuBar, &MenuBar::newFileRequested, this, &MainWindow::onNewFile);
    connect(menuBar, &MenuBar::fileOpenRequested, this, &MainWindow::onOpenFile);
    connect(menuBar, &MenuBar::saveFileRequested, this, &MainWindow::onSaveFile);
    connect(menuBar, &MenuBar::settingsRequested, this, &MainWindow::onShowSettings);
    connect(menuBar, &MenuBar::aboutRequested, this, &MainWindow::onShowAbout);

    connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

CodeEditor* MainWindow::createEditorTab(const QString &title, const QString &content, const QString &filePath) {
    CodeEditor *editor = new CodeEditor;
    editor->setFont(editorFont);
    editor->setPlainText(content);
    editor->setProperty("filePath", filePath);
    editor->setLineWrapMode(QPlainTextEdit::NoWrap);

    if (!filePath.isEmpty()) {
        editor->detectAndApplySyntaxHighlighting(filePath);
    }

    connect(editor, &QPlainTextEdit::cursorPositionChanged,
            this, &MainWindow::onCursorPositionChanged);

    connect(editor, &QPlainTextEdit::textChanged,
            this, &MainWindow::onTextChanged);

    int index = tabWidget->addTab(editor, title);
    tabWidget->setCurrentIndex(index);

    onCursorPositionChanged();

    return editor;
}

void MainWindow::closeTab(int index) {
    QWidget *page = tabWidget->widget(index);
    tabWidget->removeTab(index);
    delete page;
}

void MainWindow::onNewFile() {
    createEditorTab("Untitled");
}

void MainWindow::onOpenFile(const QString &fileName) {
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QFileInfo fileInfo(fileName);
        QString tabName = fileInfo.fileName();
        createEditorTab(tabName, file.readAll(), fileName);

        StatusBar *customStatusBar = dynamic_cast<StatusBar*>(statusBar);
        if (customStatusBar) {
            customStatusBar->showMessage("Opened: " + fileName, 5000);
        }
    } else {
        StatusBar *customStatusBar = dynamic_cast<StatusBar*>(statusBar);
        if (customStatusBar) {
            customStatusBar->showMessage("Failed to open file!", 5000);
        }
    }
}

void MainWindow::onSaveFile(bool saveAs) {
    CodeEditor *currentEditor = dynamic_cast<CodeEditor*>(tabWidget->currentWidget());
    if (!currentEditor) return;

    QString filePath = currentEditor->property("filePath").toString();

    if (filePath.isEmpty() || saveAs) {
        QString initialDir;

        if (!filePath.isEmpty() && !saveAs) {
            QFileInfo fileInfo(filePath);
            initialDir = fileInfo.absolutePath();
        } else {
            QModelIndex rootIndex = treeView->rootIndex();
            if (rootIndex.isValid()) {
                initialDir = fileSystemModel->filePath(rootIndex);
            }

            QModelIndex currentIndex = treeView->currentIndex();
            if (currentIndex.isValid()) {
                QString currentPath = fileSystemModel->filePath(currentIndex);
                QFileInfo pathInfo(currentPath);

                if (pathInfo.isFile()) {
                    initialDir = pathInfo.absolutePath();
                }
                else if (pathInfo.isDir()) {
                    initialDir = currentPath;
                }
            }

            if (initialDir.isEmpty()) {
                initialDir = QDir::homePath();
            }
        }

        filePath = QFileDialog::getSaveFileName(this, "Save File", initialDir, "All Files (*.*)");
    }

    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(currentEditor->toPlainText().toUtf8());
            file.close();
            QFileInfo fileInfo(filePath);
            tabWidget->setTabText(tabWidget->currentIndex(), fileInfo.fileName());
            currentEditor->setProperty("filePath", filePath);

            StatusBar *customStatusBar = dynamic_cast<StatusBar*>(statusBar);
            if (customStatusBar) {
                customStatusBar->showMessage("Saved: " + filePath, 5000);
            }
        } else {
            StatusBar *customStatusBar = dynamic_cast<StatusBar*>(statusBar);
            if (customStatusBar) {
                customStatusBar->showMessage("Failed to save file!", 5000);
            }
        }
    }
}

void MainWindow::onShowSettings() {
    SettingsDialog dialog(this, tabWidget, treeView, statusBar);
    dialog.exec();
}

void MainWindow::onShowAbout() {
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::onTreeViewDoubleClicked(const QModelIndex &index) {
    if (!index.isValid()) {
        return;
    }

    QString filePath = fileSystemModel->filePath(index);
    QFileInfo fileInfo(filePath);

    if (fileInfo.isFile()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            createEditorTab(fileInfo.fileName(), file.readAll(), filePath);

            StatusBar *customStatusBar = dynamic_cast<StatusBar*>(statusBar);
            if (customStatusBar) {
                customStatusBar->showMessage("Opened: " + filePath, 5000);
            }
        } else {
            StatusBar *customStatusBar = dynamic_cast<StatusBar*>(statusBar);
            if (customStatusBar) {
                customStatusBar->showMessage("Failed to open: " + filePath, 5000);
            }
        }
    } else if (fileInfo.isDir()) {
        terminal->setWorkingDirectory(filePath);
    }
}

void MainWindow::onCursorPositionChanged() {
    CodeEditor *currentEditor = dynamic_cast<CodeEditor*>(tabWidget->currentWidget());
    if (!currentEditor) return;

    QTextCursor cursor = currentEditor->textCursor();
    int line = cursor.blockNumber() + 1;
    int column = cursor.columnNumber() + 1;

    StatusBar *customStatusBar = dynamic_cast<StatusBar*>(statusBar);
    if (customStatusBar) {
        customStatusBar->setLineColumnInfo(line, column);
    }
}

void MainWindow::onTabChanged(int index) {
    if (index >= 0) {
        onCursorPositionChanged();
    }
}

void MainWindow::onTextChanged() {
    if (autoSaveEnabled) {
        startAutoSaveTimer();
    }
}

void MainWindow::startAutoSaveTimer() {
    autoSaveTimer->stop();
    autoSaveTimer->start(autoSaveInterval * 1000);
}

void MainWindow::autoSaveCurrentFile() {
    CodeEditor *currentEditor = dynamic_cast<CodeEditor*>(tabWidget->currentWidget());
    if (!currentEditor) return;

    QString filePath = currentEditor->property("filePath").toString();

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(currentEditor->toPlainText().toUtf8());
        file.close();

        StatusBar *customStatusBar = dynamic_cast<StatusBar*>(statusBar);
        if (customStatusBar) {
            customStatusBar->showMessage("Auto-saved: " + filePath, 2000);
        }
    }
}

void MainWindow::saveSettings() {
    QSettings settings("ChoraEditor", "Chora");

    QModelIndex rootIndex = treeView->rootIndex();
    if (rootIndex.isValid()) {
        QString currentPath = fileSystemModel->filePath(rootIndex);
        settings.setValue("lastFolder", currentPath);
    }

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    settings.setValue("terminalVisible", terminal->isVisible());

    settings.setValue("autoSaveEnabled", autoSaveEnabled);
    settings.setValue("autoSaveInterval", autoSaveInterval);
}

void MainWindow::restoreSettings() {
    QSettings settings("ChoraEditor", "Chora");

    QString lastFolder = settings.value("lastFolder", QDir::homePath()).toString();

    QDir dir(lastFolder);
    if (dir.exists()) {
        fileSystemModel->setRootPath(lastFolder);
        treeView->setRootIndex(fileSystemModel->index(lastFolder));
        terminal->setWorkingDirectory(lastFolder);
    } else {
        fileSystemModel->setRootPath(QDir::homePath());
        treeView->setRootIndex(fileSystemModel->index(QDir::homePath()));
        terminal->setWorkingDirectory(QDir::homePath());
    }

    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    bool terminalVisible = settings.value("terminalVisible", true).toBool();
    terminal->setVisible(terminalVisible);

    autoSaveEnabled = settings.value("autoSaveEnabled", true).toBool();
    autoSaveInterval = settings.value("autoSaveInterval", 3).toInt();
}