#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once
#include <QMainWindow>
#include <QWidget>
#include <QFont>

class QFileSystemModel;
class QTreeView;
class QTabWidget;
class QStatusBar;
class MenuBar;
class CodeEditor;
class QModelIndex;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void closeTab(int index);
    void onNewFile();
    void onOpenFile(const QString &fileName);
    void onSaveFile(bool saveAs);
    void onShowSettings();
    void onShowAbout();
    void onTreeViewDoubleClicked(const QModelIndex &index);

private:
    void setupUI();
    void setupConnections();
    CodeEditor* createEditorTab(const QString &title, const QString &content = "",
                                const QString &filePath = "");

    QFileSystemModel *fileSystemModel;
    QTreeView *treeView;
    QTabWidget *tabWidget;
    QStatusBar *statusBar;
    MenuBar *menuBar;
    QFont editorFont;
};

#endif //MAINWINDOW_H