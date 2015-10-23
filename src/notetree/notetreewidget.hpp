#ifndef NOTETREEWIDGET_HPP
#define NOTETREEWIDGET_HPP

#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QMenu>

#include <QtGui/QDropEvent>

class NoteTreeWidget: public QTreeWidget
{
    Q_OBJECT

public:
    NoteTreeWidget(QWidget *parent = 0);
    ~NoteTreeWidget();

    void dropEvent(QDropEvent *event);

signals:
    void saveTree();
    void addTreeItem(bool isCategory);

private:
    QMenu *_itemMenu;
    QMenu *_treeMenu;
};

#endif // NOTETREEWIDGET_HPP
