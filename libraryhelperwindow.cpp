#include "libraryhelperwindow.hpp"
#include "ui_libraryhelperwindow.h"

#include <QtCore/QDebug>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include <QtWidgets/QFileDialog>

LibraryHelperWindow::LibraryHelperWindow(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::LibraryHelperWindow)
{
    qDebug() << "Create instance";

    this->_ui->setupUi(this);

    connect(this->_ui->quitButton, &QPushButton::clicked, []() {
        qDebug() << "Quit application";

        qApp->quit();
    });

    connect(this->_ui->openButton, &QPushButton::clicked, [this]() {
        qDebug() << "Open existing library";

        QString libraryFilename = QFileDialog::getOpenFileName(this, tr("Open library"), "", tr("Library Files (*.lib);;All Files (*.*)"));
        QFile libraryFile(libraryFilename);
        if (!libraryFile.exists()) {
            qCritical("%s", QString("File %1 does not exist!").arg(libraryFilename).toUtf8().constData());
            return;
        }

        if (!libraryFile.open(QIODevice::ReadOnly)) {
            qCritical("%s", QString("File %1 could not be opened: %2!").arg(libraryFilename).arg(libraryFile.errorString()).toUtf8().constData());
            return;
        }

        QJsonParseError parseError;
        QJsonDocument library = QJsonDocument::fromJson(libraryFile.readAll(), &parseError);
        libraryFile.close();
        if (parseError.error != QJsonParseError::NoError) {
            qCritical("%s", QString("File %1 is not a valid json file: %2!").arg(libraryFilename).arg(parseError.errorString()).toUtf8().constData());
            return;
        }

        if (!library.isObject()) {
            qCritical("%s", QString("File %1 is not a valid json object!").arg(libraryFilename).toUtf8().constData());
            return;
        }

        qDebug() << "Successfully opened library" << libraryFilename;
        emit librarySelected(library.object(), libraryFilename);
    });

    connect(this->_ui->newButton, &QPushButton::clicked, [this]() {
        qDebug() << "Creating new library";

        QString libraryFilename = QFileDialog::getSaveFileName(this, tr("Create library"), "My Library.lib", tr("Library Files (*.lib);;All Files (*.*)"));
        QFile libraryFile(libraryFilename);
        if (libraryFile.exists()) {
            qCritical("%s", QString("File %1 already exists!").arg(libraryFilename).toUtf8().constData());
            return;
        }

        if (!libraryFile.open(QIODevice::WriteOnly)) {
            qCritical("%s", QString("File %1 could not be created: %2!").arg(libraryFilename).arg(libraryFile.errorString()).toUtf8().constData());
            return;
        }

        QJsonDocument library;
        QJsonObject root;
        root.insert("name", "My Library");
        root.insert("notes", QJsonArray());
        library.setObject(root);

        if (!libraryFile.write(library.toJson(), library.toJson().length())) {
            qCritical("%s", QString("File %1 could not be written: %2!").arg(libraryFilename).arg(libraryFile.errorString()).toUtf8().constData());
            libraryFile.close();
            return;
        }

        libraryFile.close();

        qDebug() << "Successfully created library" << libraryFilename;
        emit librarySelected(library.object(), libraryFilename);
    });

    connect(this, &LibraryHelperWindow::librarySelected, [this](QJsonObject library, QString libraryFilename) {
        qDebug() << "Opening library" << libraryFilename;
        this->_mainWindow = new MainWindow(library, libraryFilename);
        this->_mainWindow->show();
        this->close();
    });
}

LibraryHelperWindow::~LibraryHelperWindow()
{
    delete this->_ui;

    qDebug() << "Destroy instance";
}

bool LibraryHelperWindow::isValidLibrary()
{
    return false;
}
