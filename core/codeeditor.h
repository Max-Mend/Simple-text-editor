#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#pragma once
#include <QPlainTextEdit>
#include <QRect>
#include <QTextBlock>
#include <QSyntaxHighlighter>

class LineNumberArea;

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void setLineNumbersVisible(bool visible);
    bool areLineNumbersVisible() const;

    void setSyntaxHighlighter(QSyntaxHighlighter *highlighter);
    void detectAndApplySyntaxHighlighting(const QString &filePath);

protected:
    void resizeEvent(QResizeEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightCurrentLine();

private:
    LineNumberArea *lineNumberArea;
    bool lineNumbersVisible;
    QSyntaxHighlighter *syntaxHighlighter;
    QColor lineNumberAreaColor;
    QColor lineNumberTextColor;
    QColor currentLineColor;
};

#endif // CODEEDITOR_H