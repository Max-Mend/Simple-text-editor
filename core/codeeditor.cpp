#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QKeyEvent>
#include <QFileInfo>

#include "codeeditor.h"
#include "linenum.h"
#include "highlighter/cpp.h"
#include "highlighter/c.h"

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , lineNumberArea(new LineNumberArea(this))
    , lineNumbersVisible(true)
    , syntaxHighlighter(nullptr)
    , lineNumberAreaColor(QColor(40, 44, 52))
    , lineNumberTextColor(QColor(128, 128, 128))
    , currentLineColor(QColor(45, 49, 57))
{
    connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    QPalette p = palette();
    p.setColor(QPalette::Base, QColor(30, 34, 42));
    p.setColor(QPalette::Text, QColor(220, 220, 220));
    setPalette(p);

    setTabStopDistance(fontMetrics().horizontalAdvance(' ') * 4);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), lineNumberAreaColor);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    const int areaWidth = lineNumberArea->width();
    const int fontHeight = fontMetrics().height();

    int currentLine = textCursor().blockNumber();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            if (blockNumber == currentLine) {
                painter.setPen(QColor(200, 200, 200));
                QFont boldFont = painter.font();
                boldFont.setBold(true);
                painter.setFont(boldFont);
            } else {
                painter.setPen(lineNumberTextColor);
                QFont normalFont = painter.font();
                normalFont.setBold(false);
                painter.setFont(normalFont);
            }

            painter.drawText(0, top, areaWidth - 5, fontHeight,
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

int CodeEditor::lineNumberAreaWidth() {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * (digits + 1);
    return space;
}

void CodeEditor::setLineNumbersVisible(bool visible) {
    lineNumbersVisible = visible;
    lineNumberArea->setVisible(visible);
    updateLineNumberAreaWidth(0);
}

bool CodeEditor::areLineNumbersVisible() const {
    return lineNumbersVisible;
}

void CodeEditor::setSyntaxHighlighter(QSyntaxHighlighter *highlighter) {
    if (syntaxHighlighter) {
        delete syntaxHighlighter;
    }
    syntaxHighlighter = highlighter;
}

void CodeEditor::detectAndApplySyntaxHighlighting(const QString &filePath) {
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();

    if (extension == "cpp" || extension == "hpp" || extension == "h") {
        setSyntaxHighlighter(new CppHighlighter(document()));
    } else if (extension == "c" || extension == "h") {
        setSyntaxHighlighter(new CHighlighter(document()));
    }
}


void CodeEditor::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                      lineNumbersVisible ? lineNumberAreaWidth() : 0,
                                      cr.height()));
}

void CodeEditor::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        QTextCursor cursor = textCursor();
        QString currentLineText = cursor.block().text();

        int indentCount = 0;
        for (QChar ch : currentLineText) {
            if (ch == ' ') indentCount++;
            else if (ch == '\t') indentCount += 4;
            else break;
        }

        QPlainTextEdit::keyPressEvent(e);

        QString indent(indentCount, ' ');
        cursor = textCursor();
        cursor.insertText(indent);

        return;
    }

    QPlainTextEdit::keyPressEvent(e);
}

void CodeEditor::updateLineNumberAreaWidth(int) {
    setViewportMargins(lineNumbersVisible ? lineNumberAreaWidth() : 0, 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(currentLineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}