#ifndef LIBRARYHELPERWINDOW_HPP
#define LIBRARYHELPERWINDOW_HPP

#include "../mainwindow.hpp"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <QtWidgets/QWidget>

namespace Ui {
    class LibraryHelperWindow;
}

class LibraryHelperWindow: public QWidget
{
    Q_OBJECT

public:
    explicit LibraryHelperWindow(QWidget *parent = 0);
    ~LibraryHelperWindow();

    /**
     * @brief Check if last selected library is valid
     * @return
     */
    bool isValidLibrary();

protected:
    /**
     * @brief Load a library and check if it's valid
     * @param path
     * @param ok
     * @return
     */
    QJsonObject loadLibrary(QString path, bool *ok) const;

    /**
     * @brief Create a new library file at path
     * @param path
     * @return
     */
    bool saveLibrary(QString path);

    /**
     * @brief Open library and lock it
     * @param library
     * @param path
     */
    bool openLibrary(QJsonObject library, QString path);

    /**
     * @brief Load recent libraries
     */
    void loadRecentLibraries();

    /**
     * @brief Append library to list of recent libraries
     */
    void addRecentLibrary(QString path);

private:
    Ui::LibraryHelperWindow *_ui;
    MainWindow *_mainWindow;

    QList<QString> _recentLibraries;

    /**
     * @brief Lock the library
     * @param path
     */
    bool _lockLibrary(QString path) const;

    /**
     * @brief Unlock the library
     * @param path
     */
    bool _unlockLibrary(QString path) const;
};

#endif // LIBRARYHELPERWINDOW_HPP
