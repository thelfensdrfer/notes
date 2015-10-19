#include "src/library/libraryhelperwindow.hpp"

#include <QtCore/QJsonDocument>

#include <QtGui/QFontDatabase>

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("Notes");
    QCoreApplication::setApplicationVersion("0.1.0");
    QCoreApplication::setOrganizationName("VisualAppeal");
    QCoreApplication::setOrganizationDomain("visualappeal.de");

    QApplication a(argc, argv);

    if (QFontDatabase::addApplicationFont(":/fonts/opensans-regular.ttf") == -1)
        qCritical("%s", "Could not load font opensans-regular.ttf");
    if (QFontDatabase::addApplicationFont(":/fonts/opensans-bold.ttf") == -1)
        qCritical("%s", "Could not load font opensans-bold.ttf");

    if (QFontDatabase::addApplicationFont(":/fonts/sourcecodepro-regular.ttf") == -1)
        qCritical("%s", "Could not load font sourcecodepro-regular.ttf");
    if (QFontDatabase::addApplicationFont(":/fonts/sourcecodepro-bold.ttf") == -1)
        qCritical("%s", "Could not load font sourcecodepro-bold.ttf");

    a.setStyleSheet("* {font-family: 'OpenSans'; font-size: 12px;} Editor, Editor * {font-family: 'SourceCodePro'; font-size: 12px;}");

    LibraryHelperWindow libraryHelper;
    if (!libraryHelper.isValidLibrary())
        libraryHelper.show();

    return a.exec();
}
