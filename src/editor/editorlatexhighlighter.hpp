#ifndef EDITORLATEXHIGHLIGHTER_HPP
#define EDITORLATEXHIGHLIGHTER_HPP

#include "editorcolorscheme.hpp"

#include <QtCore/QObject>

#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextCharFormat>

class EditorLatexHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    EditorLatexHighlighter(EditorColorScheme *colorScheme, QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    EditorColorScheme *_colorScheme;

    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat mathDollarFormat;
    QTextCharFormat mathCenterFormat;
    QTextCharFormat backslashFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

#endif // EDITORLATEXHIGHLIGHTER_HPP
