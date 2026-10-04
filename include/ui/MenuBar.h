#ifndef MENUBAR_H
#define MENUBAR_H

#pragma once
#include <QMenu>
#include <QMenuBar>
#include <QMainWindow>
#include <QTabWidget>
#include <QTreeView>
#include <QStatusBar>
#include <QFileSystemModel>
#include <QFileDialog>
#include <QApplication>
#include <QDebug>

class MenuBar : public QObject {
    Q_OBJECT

public:
    explicit MenuBar(QMainWindow *parent, QTabWidget *tabs, QTreeView *tree,
                     QStatusBar *statusBar, QFileSystemModel *model);
    ~MenuBar();

    signals:
        void newFileRequested();
    void fileOpenRequested(const QString &filePath);
    void saveFileRequested(bool saveAs);
    void settingsRequested();
    void aboutRequested();

private slots:
    void onNewFile();
    void onOpenFile();
    void onOpenFolder();
    void onSaveFile();
    void onSaveFileAs();
    void onOpenPlugin();
    void onOpenSettings();
    void onShowAbout();

private:
    void setupFileMenu();
    void setupEditMenu();
    void setupPluginsMenu();
    void setupSettingsMenu();
    void setupHelpMenu();

    QMainWindow *mainWindow;
    QMenuBar *menuBar;
    QTabWidget *tabWidget;
    QTreeView *treeView;
    QStatusBar *statusBar;
    QFileSystemModel *fileSystemModel;
};

#endif //MENUBAR_H