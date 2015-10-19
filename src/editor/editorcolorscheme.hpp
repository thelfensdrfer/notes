#ifndef EDITORCOLORSCHEME_HPP
#define EDITORCOLORSCHEME_HPP

#include <QtGui/QColor>

class EditorColorScheme
{
public:
    EditorColorScheme(
            QColor background,
            QColor text,
            QColor highlightLine,
            QColor keyword,
            QColor lineNumber,
            QColor comment,
            QColor parameter,
            QColor formula
    );

    QColor background() const;
    QColor text() const;
    QColor highlightLine() const;
    QColor keyword() const;
    QColor lineNumber() const;
    QColor comment() const;
    QColor parameter() const;
    QColor formula() const;

private:
    QColor _background;
    QColor _text;
    QColor _highlightLine;
    QColor _keyword;
    QColor _lineNumber;
    QColor _comment;
    QColor _parameter;
    QColor _formula;
};

#endif // EDITORCOLORSCHEME_HPP
