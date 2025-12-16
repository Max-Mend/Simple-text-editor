#include "MainWindow.h"
#include "MenuBar.h"
#include "SettingsDialog.h"
#include "AboutDialog.h"
#include "StatusBar.h"
#include "../core/codeeditor.h"

#include <QFileSystemModel>
#include <QTreeView>
#include <QTabWidget>
#include <QSplitter>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    setupConnections();

    setWindowTitle("Chora spatium");
    setWindowIcon(QIcon("../assets/logo.svg"));
    resize(1200, 800);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    fileSystemModel = new QFileSystemModel(this);
    fileSystemModel->setRootPath(QDir::homePath());

    treeView = new QTreeView;
    treeView->setModel(fileSystemModel);
    treeView->setRootIndex(fileSystemModel->index(QDir::homePath()));
    treeView->setColumnHidden(1, true);
    treeView->setColumnHidden(2, true);
    treeView->setColumnHidden(3, true);
    treeView->setHeaderHidden(true);
    treeView->setMaximumWidth(300);
    treeView->setMinimumWidth(150);

    tabWidget = new QTabWidget;
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    tabWidget->setTabPosition(QTabWidget::North);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(treeView);
    splitter->addWidget(tabWidget);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 4);

    setCentralWidget(splitter);

    // Create custom status bar only once
    statusBar = new StatusBar(this);
    setStatusBar(statusBar);

    menuBar = new MenuBar(this, tabWidget, treeView, statusBar, fileSystemModel);

    editorFont = QFont("JetBrains Mono", 14);
    editorFont.setFixedPitch(true);

    createEditorTab("Untitled");
}

void MainWindow::setupConnections() {
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(treeView, &QTreeView::doubleClicked, this, &MainWindow::onTreeViewDoubleClicked);

    connect(menuBar, &MenuBar::newFileRequested, this, &MainWindow::onNewFile);
    connect(menuBar, &MenuBar::fileOpenRequested, this, &MainWindow::onOpenFile);
    connect(menuBar, &MenuBar::saveFileRequested, this, &MainWindow::onSaveFile);
    connect(menuBar, &MenuBar::settingsRequested, this, &MainWindow::onShowSettings);
    connect(menuBar, &MenuBar::aboutRequested, this, &MainWindow::onShowAbout);

    // Connect tab changes to update cursor position
    connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

CodeEditor* MainWindow::createEditorTab(const QString &title, const QString &content,
                                        const QString &filePath) {
    CodeEditor *editor = new CodeEditor;
    editor->setFont(editorFont);
    editor->setPlainText(content);
    editor->setProperty("filePath", filePath);
    editor->setLineWrapMode(QPlainTextEdit::NoWrap);

    // Connect cursor position changes
    connect(editor, &QPlainTextEdit::cursorPositionChanged,
            this, &MainWindow::onCursorPositionChanged);

    int index = tabWidget->addTab(editor, title);
    tabWidget->setCurrentIndex(index);

    // Update status bar for new tab
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
        filePath = QFileDialog::getSaveFileName(this, "Save File", "", "All Files (*.*)");
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
        }
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