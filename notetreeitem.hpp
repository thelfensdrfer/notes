#ifndef NOTETREEITEM_HPP
#define NOTETREEITEM_HPP

#include <QtCore/QJsonObject>

#include <QtWidgets/QTreeWidgetItem>

class NoteTreeItem;

class NoteTreeItem: public QTreeWidgetItem
{
public:
    enum ItemType {
        Type = 0,
        UserType = 1000,
        CategoryType = 1100,
        NoteType = 1200
    };

    static const int COLUMN_NAME = 0;

    /**
     * @brief Child item constructor
     * @param item
     */
    NoteTreeItem(QJsonObject item, NoteTreeItem::ItemType type);

    /**
     * @brief Child item constructor
     * @param name
     */
    NoteTreeItem(QString name, NoteTreeItem::ItemType type);

    ~NoteTreeItem();

    Qt::ItemFlags flags() const;

    QString content() const;
    void setContent(QString content);

    /**
     * @brief Set the currently open tab
     * @param editor
     */
    void setOpenTab(QWidget *editor = Q_NULLPTR);
    QWidget* openTab();

    QJsonObject toJson() const;

private:
    QWidget *_editor;

    QString _content;
};

#endif // NOTETREEITEM_HPP
