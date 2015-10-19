#include "editorlatexhighlighter.hpp"

#include <QtCore/QDebug>

EditorLatexHighlighter::EditorLatexHighlighter(EditorColorScheme *colorScheme, QTextDocument *parent):
    QSyntaxHighlighter(parent),
    _colorScheme(colorScheme)
{
    qDebug() << "Create instance";

    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    QStringList keywordPatterns;
    keywordPatterns << "\\bsection\\b" << "\\bsubsection\\b" << "\\bsubsubsection\\b" << "\\bchapter\\b"
            << "\\bdfrac\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    //
    // accol
    //
    functionFormat.setForeground(this->_colorScheme->parameter());
    rule.pattern = QRegExp("\\{[^\\{\\}]*\\}");
    rule.format = functionFormat;
    highlightingRules.append(rule);
    functionFormat.setFontUnderline(false);
    functionFormat.setForeground(Qt::white);
    rule.pattern = QRegExp("\\{|\\}");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    //
    // LaTeX command
    //
    classFormat.setForeground(this->_colorScheme->keyword());
    rule.pattern = QRegExp("\\\\[A-Za-z]*");
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(this->_colorScheme->comment());
    rule.pattern = QRegExp("%[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    // \ style
    backslashFormat.setForeground(this->_colorScheme->keyword());
    rule.pattern = QRegExp("\\\\");
    rule.format = backslashFormat;
    highlightingRules.append(rule);

    // $.*$ style
    mathDollarFormat.setForeground(this->_colorScheme->formula());
    rule.pattern = QRegExp("\\$[^\\$]*\\$");
    rule.format = mathDollarFormat;
    highlightingRules.append(rule);

    // \[.*\] style
    mathCenterFormat.setForeground(this->_colorScheme->parameter());
    rule.pattern = QRegExp("\\\\\\[.*\\\\\\]");
    rule.format = mathCenterFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void EditorLatexHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
