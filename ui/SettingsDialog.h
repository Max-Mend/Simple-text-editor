#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>
#include <QTreeView>
#include <QStatusBar>

class CodeEditor;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent, QTabWidget *tabs,
                           QTreeView *tree, QStatusBar *statusBar);
    ~SettingsDialog();

private slots:
    void applySettings();

private:
    void setupUI();
    void setupInterfaceGroup();
    void setupEditorGroup();
    void setupButtons();

    QTabWidget *tabWidget;
    QTreeView *treeView;
    QStatusBar *statusBar;

    QVBoxLayout *mainLayout;
    QGroupBox *interfaceGroup;
    QGroupBox *editorGroup;

    QCheckBox *treeViewCheckBox;
    QCheckBox *lineNumbersCheckBox;
    QCheckBox *statusBarCheckBox;
    QCheckBox *wordWrapCheckBox;
    QSpinBox *fontSizeSpinBox;

    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *applyButton;
};

#endif //SETTINGSDIALOG_H