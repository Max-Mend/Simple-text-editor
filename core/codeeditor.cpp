#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>

#include "codeeditor.h"
#include "linenum.h"

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , lineNumberArea(new LineNumberArea(this))
    , lineNumbersVisible(true)
{
    connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest, this, &CodeEditor::updateLineNumberArea);

    updateLineNumberAreaWidth(0);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(40, 44, 52));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    const int areaWidth = lineNumberArea->width();
    const int fontHeight = fontMetrics().height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(128, 128, 128));
            painter.drawText(0, top, areaWidth - 5, fontHeight,
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * (digits + 1);
    return space;
}

void CodeEditor::setLineNumbersVisible(bool visible)
{
    lineNumbersVisible = visible;
    lineNumberArea->setVisible(visible);
    updateLineNumberAreaWidth(0);
}

bool CodeEditor::areLineNumbersVisible() const
{
    return lineNumbersVisible;
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                      lineNumbersVisible ? lineNumberAreaWidth() : 0,
                                      cr.height()));
}

void CodeEditor::updateLineNumberAreaWidth(int)
{
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