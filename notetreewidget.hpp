#ifndef NOTETREEWIDGET_HPP
#define NOTETREEWIDGET_HPP

#include <QtWidgets/QTreeWidget>

#include <QtGui/QDropEvent>

class NoteTreeWidget: public QTreeWidget
{
    Q_OBJECT

public:
    NoteTreeWidget(QWidget *parent = 0);

    void dropEvent(QDropEvent *event);

signals:
    void itemDropped();
};

#endif // NOTETREEWIDGET_HPP
