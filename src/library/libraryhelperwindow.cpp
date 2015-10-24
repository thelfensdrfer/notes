#include "libraryhelperwindow.hpp"
#include "ui_libraryhelperwindow.h"

#include <QtCore/QDebug>
#include <QtCore/QJsonArray>
#include <QtCore/QSettings>
#include <QtCore/QDateTime>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QTableWidgetItem>

LibraryHelperWindow::LibraryHelperWindow(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::LibraryHelperWindow)
{
    qDebug() << "Create instance";

    this->_ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setStyleSheet("LibraryHelperWindow { background-color: #fff; } #titleLabel { color: #222; } #recentLibrariesTable { selection-background-color: #222; selection-color: #fff; }");

    this->_ui->titleLabel->setText(QString("%1 %2").arg(qApp->applicationName()).arg(qApp->applicationVersion()));

    this->_ui->recentLibrariesTable->setFocusPolicy(Qt::NoFocus);

    connect(this->_ui->quitButton, &QPushButton::clicked, []() {
        qDebug() << "Quit application";

        qApp->quit();
    });

    connect(this->_ui->openButton, &QPushButton::clicked, [this]() {
        qDebug() << "Open existing library";

        QString libraryFilename = QFileDialog::getOpenFileName(this, tr("Open library"), "", tr("Library Files (*.lib);;All Files (*.*)"));

        bool ok;
        QJsonObject library = this->loadLibrary(libraryFilename, &ok);
        if (!ok)
            return;

        qDebug() << "Successfully opened library" << libraryFilename;
        this->openLibrary(library, libraryFilename);
    });

    connect(this->_ui->newButton, &QPushButton::clicked, [this]() {
        qDebug() << "Creating new library";

        QString libraryFilename = QFileDialog::getSaveFileName(this, tr("Create library"), "My Library.lib", tr("Library Files (*.lib);;All Files (*.*)"));
        bool ok = this->saveLibrary(libraryFilename);
        if (!ok)
            return;

        QJsonObject library = this->loadLibrary(libraryFilename, &ok);
        if (!ok)
            return;

        qDebug() << "Successfully created library" << libraryFilename;
        this->openLibrary(library, libraryFilename);
    });

    connect(this->_ui->recentLibrariesTable, &QTableWidget::itemDoubleClicked, [this](QTableWidgetItem *item) {
        qDebug() << "Load recent library";
        if (this->_recentLibraries.count() <= item->row()) {
            qWarning("%s", QString("Library does not exist anymore").toUtf8().constData());
            return;
        }

        bool ok;
        QString path = this->_recentLibraries.at(item->row());
        QJsonObject library = this->loadLibrary(path, &ok);
        if (!ok)
            return;

        qDebug() << "Recent library loaded";
        this->openLibrary(library, path);
    });

    this->loadRecentLibraries();
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

QJsonObject LibraryHelperWindow::loadLibrary(QString path, bool *ok) const
{
    *ok = false;

    QFile libraryFile(path);
    if (!libraryFile.exists()) {
        qCritical("%s", QString("File %1 does not exist!").arg(path).toUtf8().constData());
        return QJsonObject();
    }

    QFile libraryLockFile(path.append(".lock"));
    if (libraryLockFile.exists()) {
        qCritical("%s", QString("File %1 is locked by another user").arg(path).toUtf8().constData());
        return QJsonObject();
    }

    if (!libraryFile.open(QIODevice::ReadOnly)) {
        qCritical("%s", QString("File %1 could not be opened: %2!").arg(path).arg(libraryFile.errorString()).toUtf8().constData());
        return QJsonObject();
    }

    QJsonParseError parseError;
    QJsonDocument library = QJsonDocument::fromJson(libraryFile.readAll(), &parseError);
    libraryFile.close();
    if (parseError.error != QJsonParseError::NoError) {
        qCritical("%s", QString("File %1 is not a valid json file: %2!").arg(path).arg(parseError.errorString()).toUtf8().constData());
        return QJsonObject();
    }

    if (!library.isObject()) {
        qCritical("%s", QString("File %1 is not a valid json object!").arg(path).toUtf8().constData());
        return QJsonObject();
    }

    *ok = true;
    return library.object();
}

bool LibraryHelperWindow::saveLibrary(QString path)
{
    QFile libraryFile(path);
    if (libraryFile.exists()) {
        qCritical("%s", QString("File %1 already exists!").arg(path).toUtf8().constData());
        return false;
    }

    if (!libraryFile.open(QIODevice::WriteOnly)) {
        qCritical("%s", QString("File %1 could not be created: %2!").arg(path).arg(libraryFile.errorString()).toUtf8().constData());
        return false;
    }

    QFileInfo libraryFileInfo(path);

    QJsonDocument library;
    QJsonObject root;
    root.insert("name", libraryFileInfo.baseName());
    root.insert("notes", QJsonArray());
    library.setObject(root);

    if (!libraryFile.write(library.toJson(), library.toJson().length())) {
        qCritical("%s", QString("File %1 could not be written: %2!").arg(path).arg(libraryFile.errorString()).toUtf8().constData());
        libraryFile.close();
        return false;
    }

    libraryFile.close();

    return true;
}

bool LibraryHelperWindow::openLibrary(QJsonObject library, QString path)
{
    if (!this->_lockLibrary(path))
        return false;

    qDebug() << "Opening library" << path;
    this->addRecentLibrary(path);

    this->_mainWindow = new MainWindow(library, path);

    connect(this->_mainWindow, &MainWindow::closing, [this](QJsonObject library, QString path) {
        Q_UNUSED(library);

        this->_unlockLibrary(path);
    });

    this->_mainWindow->show();
    this->close();

    return true;
}

void LibraryHelperWindow::loadRecentLibraries()
{
    qDebug() << "Load recent libraries";
    this->_ui->recentLibrariesTable->clear();

    QSettings settings;
    int count = settings.beginReadArray("recent_libraries");
    this->_ui->recentLibrariesTable->setRowCount(count);
    this->_ui->recentLibrariesTable->setColumnCount(2);

    for (int i = 0; i < count; i++) {
        settings.setArrayIndex(i);
        QString path = settings.value("path").toString();
        QFile file(path);
        if (!file.exists()) {
            qWarning("%s", QString("File %1 does not exist anymore!").arg(path).toUtf8().constData());
            settings.remove("path");
            continue;
        }

        if (!file.open(QIODevice::ReadOnly)) {
            qCritical("%s", QString("Could not read file %1: %2!").arg(path).arg(file.errorString()).toUtf8().constData());
            settings.remove("path");
            continue;
        }

        QJsonDocument library = QJsonDocument::fromJson(file.readAll());
        if (!library.isObject() || !library.object().contains("name")) {
            qWarning("%s", QString("Library %s is not a valid json object or does not have a name key!").arg(path).toUtf8().constBegin());
            settings.remove("path");
            continue;
        }

        QFileInfo fileInfo(path);

        QTableWidgetItem *name = new QTableWidgetItem(library.object().value("name").toString());
        this->_ui->recentLibrariesTable->setItem(i, 0, name);

        QTableWidgetItem *lastChanged = new QTableWidgetItem(fileInfo.lastModified().toString("yyyy-MM-dd HH:mm"));
        lastChanged->setData(Qt::UserRole, fileInfo.lastModified());
        lastChanged->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        this->_ui->recentLibrariesTable->setItem(i, 1, lastChanged);

        this->_recentLibraries.append(path);
        qDebug() << "Loaded recent library" << path;
    }
    settings.endArray();

    this->_ui->recentLibrariesTable->sortItems(1, Qt::DescendingOrder);
}

void LibraryHelperWindow::addRecentLibrary(QString path)
{
    qDebug() << "Add recent library" << path;

    // Check if already added
    for (int i = 0; i < this->_recentLibraries.count(); i++) {
        if (this->_recentLibraries.at(i) == path) {
            qDebug() << "Recent library already added";
            return;
        }
    }

    this->_recentLibraries.append(path);

    QSettings settings;
    qDebug() << settings.fileName();
    settings.beginWriteArray("recent_libraries");

    for (int i = 0; i < this->_recentLibraries.count(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("path", this->_recentLibraries.at(i));
    }
    settings.endArray();
}

bool LibraryHelperWindow::_lockLibrary(QString path) const
{
    qDebug() << "Lock library" << path;

    QString lockFileName = path.append(".lock");

    QFile libraryLockFile(lockFileName);
    if (libraryLockFile.exists()) {
        qCritical("%s", QString("File %1 is already locked by another user!").arg(path).toUtf8().constData());
        return false;
    }

    if (!libraryLockFile.open(QIODevice::WriteOnly)) {
        qCritical("%s", QString("Could not open lock file: %1!").arg(lockFileName).toUtf8().constData());
        return false;
    }

    libraryLockFile.write("");
    libraryLockFile.close();

    return true;
}

bool LibraryHelperWindow::_unlockLibrary(QString path) const
{
    qDebug() << "Unlock library" << path;

    QString lockFileName = path.append(".lock");

    QFile libraryLockFile(lockFileName);
    if (!libraryLockFile.exists()) {
        qCritical("%s", QString("File %1 is not locked by another user!").arg(path).toUtf8().constData());
        return false;
    }

    if (!libraryLockFile.remove()) {
        qCritical("%s", QString("File %1 lock could not be removed!").arg(lockFileName).toUtf8().constData());
        return false;
    }

    return true;
}
