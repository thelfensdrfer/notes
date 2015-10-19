#include "editor.hpp"

#include <QtCore/QDebug>

#include <QtGui/QTextBlock>
#include <QtGui/QPainter>

Editor::Editor(NoteTreeItem *note, EditorColorScheme *colorScheme, QWidget *parent):
    QPlainTextEdit(parent),
    _note(note),
    _colorScheme(colorScheme),
    _dirty(false),
    _lineNumbers(true)
{
    qDebug() << "Create instance" << note->text(NoteTreeItem::COLUMN_NAME);

    // Setup editor
    this->setPlainText(note->content());
    this->setPlaceholderText(tr("Your notes"));

    this->setStyleSheet(QString("background-color: %1; color: %2;").arg(this->_colorScheme->background().name()).arg(this->_colorScheme->text().name()));

    // Line numbers widget
    this->_lineNumersWidget = new EditorLineNumbersWidget(this);

    connect(this, &Editor::blockCountChanged, this, &Editor::_updateLineNumberAreaWidth);
    connect(this, &Editor::updateRequest, this, &Editor::_updateLineNumberArea);
    connect(this, &Editor::cursorPositionChanged, this, &Editor::_highlightCurrentLine);

    // Syntax highlighter
    this->_latexHighlighter = new EditorLatexHighlighter(this->_colorScheme, this->document());

    this->_updateLineNumberAreaWidth(0);
    this->_highlightCurrentLine();

    connect(this, &Editor::textChanged, [this]() {
        qDebug() << "Text changed";
        this->_dirty = true;
        emit dirtyChanged(this->_dirty);
    });
}

Editor::~Editor()
{
    delete this->_lineNumersWidget;
    delete this->_latexHighlighter;
}

void Editor::save()
{
    qDebug() << "Saving editor contents";

    this->_note->setContent(this->toPlainText());
    this->_dirty = false;
    emit dirtyChanged(this->_dirty);
}

bool Editor::isDirty()
{
    return this->_dirty;
}

NoteTreeItem* Editor::note()
{
    return this->_note;
}

void Editor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(this->_lineNumersWidget);
    painter.fillRect(event->rect(), this->_colorScheme->background());

    QTextBlock block = this->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) this->blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) this->blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(this->_colorScheme->lineNumber());
            painter.drawText(0, top, this->_lineNumersWidget->width(), fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) this->blockBoundingRect(block).height();
        blockNumber++;
    }
}

int Editor::lineNumberAreaWidth() const
{
    int digits = 1;
    int max = qMax(1, this->blockCount());
    while (max >= 10) {
        max /= 10;
        digits++;
    }

    int space = 3 + this->fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void Editor::setColorScheme(EditorColorScheme *colorScheme)
{
    this->_colorScheme = colorScheme;
}

void Editor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    this->_lineNumersWidget->setGeometry(QRect(cr.left(), cr.top(), this->lineNumberAreaWidth(), cr.height()));
}

void Editor::_updateLineNumberAreaWidth(int newBlockCount)
{
    Q_UNUSED(newBlockCount);

    this->setViewportMargins(this->lineNumberAreaWidth(), 0, 0, 0);
}

void Editor::_highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        selection.format.setBackground(this->_colorScheme->highlightLine());
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    this->setExtraSelections(extraSelections);
}

void Editor::_updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        this->_lineNumersWidget->scroll(0, dy);
    else
        this->_lineNumersWidget->update(0, rect.y(), this->_lineNumersWidget->width(), rect.height());

    if (rect.contains(this->viewport()->rect()))
        this->setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
