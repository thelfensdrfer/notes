#include "src/library/libraryhelperwindow.hpp"

#include <QtCore/QJsonDocument>

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LibraryHelperWindow libraryHelper;
    if (!libraryHelper.isValidLibrary())
        libraryHelper.show();

    return a.exec();
}
