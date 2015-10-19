#include "editorcolorscheme.hpp"

#include <QtCore/QDebug>

EditorColorScheme::EditorColorScheme(
        QColor background,
        QColor text,
        QColor highlightLine,
        QColor keyword,
        QColor lineNumber,
        QColor comment,
        QColor parameter,
        QColor formula
    ):
    _background(background),
    _text(text),
    _highlightLine(highlightLine),
    _keyword(keyword),
    _lineNumber(lineNumber),
    _comment(comment),
    _parameter(parameter),
    _formula(formula)
{
    qDebug() << "Create instance";
}

QColor EditorColorScheme::background() const
{
    return this->_background;
}

QColor EditorColorScheme::text() const
{
    return this->_text;
}

QColor EditorColorScheme::highlightLine() const
{
    return this->_highlightLine;
}

QColor EditorColorScheme::keyword() const
{
    return this->_keyword;
}

QColor EditorColorScheme::lineNumber() const
{
    return this->_lineNumber;
}

QColor EditorColorScheme::comment() const
{
    return this->_comment;
}

QColor EditorColorScheme::parameter() const
{
    return this->_parameter;
}

QColor EditorColorScheme::formula() const
{
    return this->_formula;
}
