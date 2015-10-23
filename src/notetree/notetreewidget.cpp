#include "notetreewidget.hpp"

#include <QtCore/QDebug>

NoteTreeWidget::NoteTreeWidget(QWidget *parent):
    QTreeWidget(parent)
{
    qDebug() << "Create instance";

    this->_itemMenu = new QMenu(this);

    QAction *deleteAction = new QAction(tr("Delete"), this->_itemMenu);
    this->_itemMenu->addAction(deleteAction);

    QAction *insertItemAction = new QAction(tr("Create note"), this->_itemMenu);
    this->_itemMenu->addAction(insertItemAction);

    this->_itemMenu->insertSeparator(insertItemAction);

    QAction *insertCategoryAction = new QAction(tr("Create category"), this->_itemMenu);
    this->_itemMenu->addAction(insertCategoryAction);

    this->_treeMenu = new QMenu(this);

    this->_treeMenu->addAction(insertItemAction);
    this->_treeMenu->addAction(insertCategoryAction);

    connect(this, &NoteTreeWidget::customContextMenuRequested, [this](const QPoint pos) {
        qDebug() << "Requested note tree context menu";

        if (this->selectedItems().count() > 0)
            this->_itemMenu->exec(this->viewport()->mapToGlobal(pos));
        else
            this->_treeMenu->exec(this->viewport()->mapFromGlobal(pos));
    });

    connect(deleteAction, &QAction::triggered, [this]() {
        qDebug() << "Delete current item";

        QTreeWidgetItem *item = this->selectedItems().first();
        if (!item)
            return;

        delete item;

        emit saveTree();
    });

    connect(insertItemAction, &QAction::triggered, [this]() {
        qDebug() << "Insert item";

        emit addTreeItem(false);
    });

    connect(insertCategoryAction, &QAction::triggered, [this]() {
        qDebug() << "Insert category";

        emit addTreeItem(true);
    });
}

NoteTreeWidget::~NoteTreeWidget()
{
    qDebug() << "Destroy instance";
}

void NoteTreeWidget::dropEvent(QDropEvent *event)
{
    QTreeWidget::dropEvent(event);

    emit saveTree();
}
