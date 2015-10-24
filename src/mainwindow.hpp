#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "src/notetree/notetreeitem.hpp"
#include "src/editor/editor.hpp"

#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QTimer>
#include <QtCore/QProcess>

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTreeWidgetItem>

#include <QtGui/QCloseEvent>

namespace Ui {
    class MainWindow;
}

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QJsonObject library, QString path, QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);

protected:
    /**
     * @brief Save tree data to class and file
     */
    bool saveTree();

    /**
     * @brief Save notes and tree to class and file
     * @return
     */
    bool saveNotes();

    /**
     * @brief Save all class data into file
     * @return
     */
    bool save(QJsonDocument doc);

    /**
     * @brief Load data from class
     */
    bool load();

    /**
     * @brief Load documents into tree
     * @param documents
     */
    void loadNotes(QJsonArray notes);

    /**
     * @brief Add a category or note to the tree
     * @param category
     */
    void addTreeItem(bool isCategory);

    /**
     * @brief Get the current active editor
     * @return Q_NULLPTR|Editor*
     */
    Editor* currentEditor();

    /**
     * @brief Convert latex to html
     * @param latex
     * @return
     */
    void latexToHtml(Editor *editor);

    /**
     * @brief Convert contents
     * @param contents
     */
    void previewContents(QUrl url);

private:
    Ui::MainWindow *_ui;
    QProcess *_convertProcess;

    QJsonObject _library;
    QString _path;
    QTimer _saveTimer;
    QList<EditorColorScheme*> _colorSchemes;

signals:
    /**
     * @brief Emitted before the window is closed
     * @param library
     * @param path
     */
    void closing(QJsonObject library, QString path);
};

#endif // MAINWINDOW_HPP
