#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once
#include <QMainWindow>
#include <QWidget>
#include <QFont>
#include <QSettings>
#include <QTimer>

class QFileSystemModel;
class QTreeView;
class QTabWidget;
class QStatusBar;
class QSplitter;
class MenuBar;
class CodeEditor;
class QModelIndex;
class TerminalWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    TerminalWidget *terminal;

    bool autoSaveEnabled;
    int autoSaveInterval;

private slots:
    void closeTab(int index);
    void onNewFile();
    void onOpenFile(const QString &fileName);
    void onSaveFile(bool saveAs);
    void onShowSettings();
    void onShowAbout();
    void onTreeViewDoubleClicked(const QModelIndex &index);
    void onCursorPositionChanged();
    void onTabChanged(int index);
    void onTextChanged();
    void autoSaveCurrentFile();

private:
    void setupUI();
    void setupConnections();
    void saveSettings();
    void restoreSettings();
    void startAutoSaveTimer();

    CodeEditor* createEditorTab(const QString &title, const QString &content = "",
                                const QString &filePath = "");

    QFileSystemModel *fileSystemModel;
    QTreeView *treeView;
    QTabWidget *tabWidget;
    QStatusBar *statusBar;
    MenuBar *menuBar;
    QFont editorFont;

    QSplitter *mainSplitter;
    QSplitter *editorSplitter;

    QTimer *autoSaveTimer;
};

#endif //MAINWINDOW_H