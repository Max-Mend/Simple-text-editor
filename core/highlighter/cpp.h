#ifndef CPP_H
#define CPP_H

#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QPlainTextEdit>

class CppHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    CppHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
        HighlightingRule rule;

        QColor keywordColor(198, 120, 221);
        QColor typeColor(209, 154, 102);
        QColor functionColor(97, 175, 239);
        QColor stringColor(152, 195, 121);
        QColor numberColor(209, 154, 102);
        QColor commentColor(92, 99, 112);
        QColor preprocessorColor(229, 192, 123);

        keywordFormat.setForeground(keywordColor);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns = {
            "\\bchar\\b", "\\bclass\\b", "\\bconst\\b", "\\bdouble\\b",
            "\\benum\\b", "\\bexplicit\\b", "\\bfriend\\b", "\\binline\\b",
            "\\bint\\b", "\\blong\\b", "\\bnamespace\\b", "\\boperator\\b",
            "\\bprivate\\b", "\\bprotected\\b", "\\bpublic\\b", "\\bshort\\b",
            "\\bsignals\\b", "\\bsigned\\b", "\\bslots\\b", "\\bstatic\\b",
            "\\bstruct\\b", "\\btemplate\\b", "\\btypedef\\b", "\\btypename\\b",
            "\\bunion\\b", "\\bunsigned\\b", "\\bvirtual\\b", "\\bvoid\\b",
            "\\bvolatile\\b", "\\bbool\\b", "\\bif\\b", "\\belse\\b",
            "\\bfor\\b", "\\bwhile\\b", "\\bdo\\b", "\\bswitch\\b",
            "\\bcase\\b", "\\bdefault\\b", "\\bbreak\\b", "\\bcontinue\\b",
            "\\breturn\\b", "\\btry\\b", "\\bcatch\\b", "\\bthrow\\b",
            "\\bnew\\b", "\\bdelete\\b", "\\bthis\\b", "\\bauto\\b",
            "\\bconstexpr\\b", "\\bdecltype\\b", "\\bnoexcept\\b"
        };
        for (const QString &pattern : keywordPatterns) {
            rule.pattern = QRegularExpression(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        typeFormat.setForeground(typeColor);
        typeFormat.setFontWeight(QFont::Bold);
        QStringList typePatterns = {"\\bint\\b", "\\bfloat\\b", "\\bdouble\\b", "\\bchar\\b", "\\bbool\\b", "\\bvoid\\b"};
        for (const QString &pattern : typePatterns) {
            rule.pattern = QRegularExpression(pattern);
            rule.format = typeFormat;
            highlightingRules.append(rule);
        }

        classFormat.setForeground(typeColor);
        classFormat.setFontWeight(QFont::Bold);
        rule.pattern = QRegularExpression("\\bQ[A-Z][A-Za-z0-9_]+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        preprocessorFormat.setForeground(preprocessorColor);
        rule.pattern = QRegularExpression("^\\s*#[^\n]*");
        rule.format = preprocessorFormat;
        highlightingRules.append(rule);

        singleLineCommentFormat.setForeground(commentColor);
        singleLineCommentFormat.setFontItalic(true);
        rule.pattern = QRegularExpression("//[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        multiLineCommentFormat.setForeground(commentColor);
        multiLineCommentFormat.setFontItalic(true);
        commentStartExpression = QRegularExpression("/\\*");
        commentEndExpression = QRegularExpression("\\*/");

        quotationFormat.setForeground(stringColor);
        rule.pattern = QRegularExpression("\"([^\"\\\\]|\\\\.)*\"");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        rule.pattern = QRegularExpression("'([^'\\\\]|\\\\.)*'");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        functionFormat.setForeground(functionColor);
        rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
        rule.format = functionFormat;
        highlightingRules.append(rule);

        numberFormat.setForeground(numberColor);
        rule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*\\b");
        rule.format = numberFormat;
        highlightingRules.append(rule);
    }

protected:
    void highlightBlock(const QString &text) override {
        for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
            QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
            while (matchIterator.hasNext()) {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }

        setCurrentBlockState(0);
        int startIndex = 0;
        if (previousBlockState() != 1)
            startIndex = text.indexOf(commentStartExpression);

        while (startIndex >= 0) {
            QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
            int endIndex = match.capturedStart();
            int commentLength = 0;
            if (endIndex == -1) {
                setCurrentBlockState(1);
                commentLength = text.length() - startIndex;
            } else {
                commentLength = endIndex - startIndex + match.capturedLength();
            }
            setFormat(startIndex, commentLength, multiLineCommentFormat);
            startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
        }
    }

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat typeFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat preprocessorFormat;
    QTextCharFormat numberFormat;
};

#endif // CPP_H
