#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#pragma once

#include <QPlainTextEdit>
#include <QRect>
#include <QTextBlock>

class LineNumberArea;

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void setLineNumbersVisible(bool visible);
    bool areLineNumbersVisible() const;

protected:
    void resizeEvent(QResizeEvent *e) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    LineNumberArea *lineNumberArea;
    bool lineNumbersVisible;
};

#endif // CODEEDITOR_H