#include "notetreeitem.hpp"

#include <QtCore/QDebug>
#include <QtCore/QJsonArray>

NoteTreeItem::NoteTreeItem(QJsonObject item, NoteTreeItem::ItemType type):
    QTreeWidgetItem(type),
    _editor(Q_NULLPTR)
{
    qDebug() << "Create instance";

    this->setText(NoteTreeItem::COLUMN_NAME, item.value("name").toString(""));
    this->_content = item.value("content").toString("");

    QJsonArray children;
    if (item.value("notes").isArray())
        children = item.value("notes").toArray();

    for (int i = 0; i < children.count(); i++) {
        QJsonObject child;
        if (children.at(i).isObject())
            child = children.at(i).toObject();

        NoteTreeItem::ItemType type;
        if (child.value("category").toBool(false))
            type = NoteTreeItem::CategoryType;
        else
            type = NoteTreeItem::NoteType;

        this->addChild(new NoteTreeItem(child, type));
    }

    if (this->type() == NoteTreeItem::CategoryType)
        this->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
    else if (this->type() == NoteTreeItem::NoteType)
        this->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);
}

NoteTreeItem::NoteTreeItem(QString name, NoteTreeItem::ItemType type):
    QTreeWidgetItem(type),
    _editor(Q_NULLPTR)
{
    qDebug() << "Create instance";

    this->setText(0, name);

    if (this->type() == NoteTreeItem::CategoryType)
        this->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
    else if (this->type() == NoteTreeItem::NoteType)
        this->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);
}

NoteTreeItem::~NoteTreeItem()
{
    qDebug() << "Destroy instance";
}

QString NoteTreeItem::content() const
{
    return this->_content;
}

void NoteTreeItem::setContent(QString content)
{
    this->_content = content;
}

bool NoteTreeItem::isCategory()
{
    return (this->type() == NoteTreeItem::CategoryType);
}

void NoteTreeItem::setOpenTab(QWidget *editor)
{
    this->_editor = editor;
}

QWidget* NoteTreeItem::openTab()
{
    return this->_editor;
}

QJsonObject NoteTreeItem::toJson() const
{
    QJsonObject item;
    item.insert("name", this->text(NoteTreeItem::COLUMN_NAME));
    item.insert("category", (this->type() == NoteTreeItem::CategoryType) ? true : false);

    if (this->type() == NoteTreeItem::NoteType) {
        qDebug() << "Save note";
        item.insert("content", this->_content);
        return item;
    }

    qDebug() << "Save category";

    // Append children
    QJsonArray children;
    for (int i = 0; i < this->childCount(); i++) {
        NoteTreeItem *note = dynamic_cast<NoteTreeItem*>(this->child(i));
        children.append(note->toJson());
    }

    item.insert("notes", children);
    return item;
}
