#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "../notetree/notetreeitem.hpp"
#include "editorcolorscheme.hpp"
#include "editorlatexhighlighter.hpp"

#include <QtCore/QObject>
#include <QtCore/QDebug>

#include <QtWidgets/QPlainTextEdit>

QT_FORWARD_DECLARE_CLASS(EditorLineNumbersWidget)

class Editor: public QPlainTextEdit
{
    Q_OBJECT

public:
    Editor(NoteTreeItem *note, EditorColorScheme *colorScheme, QWidget *parent = 0);
    ~Editor();

    void save();

    /**
     * @brief Check if unsaved changes are present
     * @return
     */
    bool isDirty();

    /**
     * @brief Get the tree item of the editor
     * @return
     */
    NoteTreeItem* note();

    /**
     * @brief Paints the line numbers
     * @param event
     */
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    /**
     * @brief Get the width of the line number area
     * @return
     */
    int lineNumberAreaWidth() const;

    /**
     * @brief Set the current color scheme
     * @param colorScheme
     */
    void setColorScheme(EditorColorScheme *colorScheme);

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    NoteTreeItem *_note;
    QWidget *_lineNumersWidget;
    EditorColorScheme *_colorScheme;
    EditorLatexHighlighter *_latexHighlighter;

    bool _dirty;
    bool _lineNumbers;

signals:
    void dirtyChanged(bool isDirty);

private slots:
    void _updateLineNumberAreaWidth(int newBlockCount);
    void _highlightCurrentLine();
    void _updateLineNumberArea(const QRect &rect, int dy);
};

class EditorLineNumbersWidget: public QWidget
{
public:
    EditorLineNumbersWidget(Editor *editor):
        QWidget(editor),
        _editor(editor)
    {
        //
    }

    QSize sizeHint() const Q_DECL_OVERRIDE
    {
        return QSize(this->_editor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE
    {
        this->_editor->lineNumberAreaPaintEvent(event);
    }

private:
    Editor *_editor;
};

#endif // EDITOR_HPP
