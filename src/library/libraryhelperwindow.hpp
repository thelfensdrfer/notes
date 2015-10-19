#ifndef LIBRARYHELPERWINDOW_HPP
#define LIBRARYHELPERWINDOW_HPP

#include "../mainwindow.hpp"

#include <QtCore/QJsonDocument>

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

private:
    Ui::LibraryHelperWindow *_ui;
    MainWindow *_mainWindow;

signals:
    /**
     * @brief Emitted when a library was selected
     * @param library
     * @param path
     */
    void librarySelected(QJsonObject library, QString path);
};

#endif // LIBRARYHELPERWINDOW_HPP
