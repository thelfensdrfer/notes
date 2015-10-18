#include "notetreewidget.hpp"

#include <QtCore/QDebug>

NoteTreeWidget::NoteTreeWidget(QWidget *parent):
    QTreeWidget(parent)
{
    qDebug() << "Create instance";
}

void NoteTreeWidget::dropEvent(QDropEvent *event)
{
    QTreeWidget::dropEvent(event);

    emit itemDropped();
}
