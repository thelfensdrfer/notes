#include "src/library/libraryhelperwindow.hpp"

#include <QtCore/QJsonDocument>

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("Notes");
    QCoreApplication::setApplicationVersion("0.1.0");
    QCoreApplication::setOrganizationName("VisualAppeal");
    QCoreApplication::setOrganizationDomain("visualappeal.de");

    QApplication a(argc, argv);

    LibraryHelperWindow libraryHelper;
    if (!libraryHelper.isValidLibrary())
        libraryHelper.show();

    return a.exec();
}
