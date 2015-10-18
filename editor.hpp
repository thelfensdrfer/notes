#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "notetreeitem.hpp"

#include <QtCore/QObject>

#include <QtWidgets/QTextEdit>

class Editor: public QTextEdit
{
    Q_OBJECT

public:
    Editor(NoteTreeItem *note, QWidget *parent = 0);

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

private:
    bool _dirty;
    NoteTreeItem *_note;
};

#endif // EDITOR_HPP
