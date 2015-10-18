#include "editor.hpp"

#include <QtCore/QDebug>

Editor::Editor(NoteTreeItem *note, QWidget *parent):
    QTextEdit(parent),
    _dirty(false),
    _note(note)
{
    qDebug() << "Create instance" << note->text(NoteTreeItem::COLUMN_NAME);

    this->setAcceptRichText(false);

    this->setPlainText(note->content());
    this->setPlaceholderText(tr("Your notes"));

    connect(this, &Editor::textChanged, [this]() {
        this->_dirty = true;
    });
}

void Editor::save()
{
    qDebug() << "Saving editor contents";

    this->_note->setContent(this->toPlainText());
    this->_dirty = false;
}

bool Editor::isDirty()
{
    return this->_dirty;
}

NoteTreeItem* Editor::note()
{
    return this->_note;
}
