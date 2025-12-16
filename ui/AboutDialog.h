#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#pragma once
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

class AboutDialog : public QDialog {
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private:
    void setupUI();
};

#endif //ABOUTDIALOG_H