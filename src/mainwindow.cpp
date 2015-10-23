#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "editor/editor.hpp"
#include "notetree/notetreewidget.hpp"

#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QUuid>

#include <QtWidgets/QMessageBox>

MainWindow::MainWindow(QJsonObject library, QString path, QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _convertProcess(new QProcess),
    _library(library),
    _path(path)
{
    qDebug() << "Create instance";

    this->_ui->setupUi(this);

    this->load();
    this->_ui->notesTree->expandAll();

    connect(this->_ui->addCategoryButton, &QPushButton::clicked, [this]() {
        this->addTreeItem(true);
    });

    connect(this->_ui->addNoteButton, &QPushButton::clicked, [this]() {
        this->addTreeItem(false);
    });

    connect(this->_ui->notesTree, &NoteTreeWidget::itemChanged, [this]() {
        qDebug() << "Item changed";

        this->saveTree();
    });

    connect(this->_ui->notesTree, &NoteTreeWidget::itemDoubleClicked, [this](QTreeWidgetItem *item, int column) {
        Q_UNUSED(column);

        NoteTreeItem *note = dynamic_cast<NoteTreeItem*>(item);
        if (!note) {
            qCritical("Could not convert tree item to note tree item");
            return;
        }

        if (note->isCategory())
            return;

        // Check if tab is already open and switch to it
        if (note->openTab() != Q_NULLPTR) {
            this->_ui->editorTabWidget->setCurrentWidget(note->openTab());
            return;
        }

        Editor *editor = new Editor(note, this->_colorSchemes.at(0));

        connect(editor, &Editor::dirtyChanged, [this, editor]() {
            if (editor->isDirty())
                this->_ui->editorTabWidget->setTabText(
                            this->_ui->editorTabWidget->indexOf(editor),
                            QString("%1 *").arg(editor->note()->text(NoteTreeItem::COLUMN_NAME))
                );
            else
                this->_ui->editorTabWidget->setTabText(
                            this->_ui->editorTabWidget->indexOf(editor),
                            QString("%1").arg(editor->note()->text(NoteTreeItem::COLUMN_NAME))
                );
        });

        this->_ui->editorTabWidget->addTab(editor, note->text(NoteTreeItem::COLUMN_NAME));
        note->setOpenTab(editor);
        this->latexToHtml(editor);

        this->_ui->menuEditor->setEnabled(true);
        this->_ui->menuPreview->setEnabled(true);
    });

    connect(this->_ui->notesTree, &NoteTreeWidget::saveTree, [this]() {
        qDebug() << "Tree should be saved";

        this->saveTree();
    });

    connect(this->_ui->notesTree, &NoteTreeWidget::addTreeItem, [this](bool isCategory) {
        this->addTreeItem(isCategory);
    });

    connect(this->_ui->editorTabWidget, &QTabWidget::tabCloseRequested, [this](int index) {
        qDebug() << "Requested tab close" << index;

        Editor *editor = dynamic_cast<Editor*>(this->_ui->editorTabWidget->widget(index));
        if (!editor) {
            qCritical("Could not convert tab to editor");
            return;
        }

        if (editor->isDirty()) {
            if (QMessageBox::question(this, tr("Do you want to save your changes?"), tr("You have unsaved changes. Do you want so save them now?"), QMessageBox::Save, QMessageBox::No))
                editor->save();
        }

        editor->note()->setOpenTab(Q_NULLPTR);
        this->_ui->editorTabWidget->removeTab(index);
        this->_ui->webView->load(QUrl("about:blank"));
        delete editor;

        if (this->_ui->editorTabWidget->count() == 0) {
            this->_ui->menuEditor->setEnabled(false);
            this->_ui->menuPreview->setEnabled(false);
        }
    });

    connect(&this->_saveTimer, &QTimer::timeout, [this]() {
        qDebug() << "Autosave";

        this->_ui->statusBar->showMessage(tr("Autosaved notes"), 3 * 1000);
        this->saveNotes();
    });

    connect(this->_convertProcess, static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::error), [this](QProcess::ProcessError error) {
        qWarning("%s", QString("Error converting latex to html: %1!").arg("Error").toUtf8().constData());
        this->_ui->convertLogEdit->appendPlainText(QString("Error: %1").arg(QString::number(error)));
    });

    connect(this->_convertProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [this](int exitCode, QProcess::ExitStatus exitStatus) {
        qDebug() << "Converted latex to html" << exitCode << exitStatus;
        if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
            this->_ui->convertLogEdit->appendPlainText(QString("Converted latex to html."));

            QString texFilePath = this->_convertProcess->arguments().first();
            QString htmlFilename = texFilePath.replace(".tex", ".html");
            QFile htmlFile(htmlFilename);
            if (!htmlFile.exists()) {
                qCritical("%s", QString("HTML file does not exist: %1!").arg(htmlFilename).toUtf8().constData());
                return;
            }

            QUrl htmlUrl = QUrl::fromLocalFile(htmlFile.fileName());
            this->previewContents(htmlUrl);
        } else {
            this->_ui->convertLogEdit->appendPlainText(QString("Could not convert latex to html! Please see the errors in the log."));
        }
    });

    connect(this->_convertProcess, &QProcess::readyRead, [this]() {
        this->_ui->convertLogEdit->appendPlainText(this->_convertProcess->readAll());
    });

    connect(this->_ui->actionReload, &QAction::triggered, [this]() {
        qDebug() << "Reload preview";
        Editor *editor = this->currentEditor();
        if (!editor)
            return;

        this->latexToHtml(editor);
    });

    EditorColorScheme *monokaiDark = new EditorColorScheme(
                QColor::fromRgb(36, 36, 36),
                QColor::fromRgb(255, 255, 255),
                QColor::fromRgb(60, 63, 65),
                QColor::fromRgb(249, 37, 72),
                QColor::fromRgb(118, 118, 118),
                QColor::fromRgb(118, 118, 118),
                QColor::fromRgb(166, 226, 46),
                QColor::fromRgb(230, 219, 116)
    );
    this->_colorSchemes.append(monokaiDark);

    // Set splitter size
    QList<int> splitterSizes;
    splitterSizes << this->width() / 6 * 1 << this->width() / 6 * 3 << this->width() / 6 * 2;
    this->_ui->splitter->setSizes(splitterSizes);

    this->_ui->splitter->setStretchFactor(0, 1);
    this->_ui->splitter->setStretchFactor(1, 3);
    this->_ui->splitter->setStretchFactor(2, 2);

    this->_ui->convertLogEdit->setFont(QFont("Source Code Pro"));

    this->_saveTimer.setInterval(30 * 1000);
    this->_saveTimer.start();
}

MainWindow::~MainWindow()
{
    delete this->_ui;

    for (int i = 0; i < this->_colorSchemes.count(); i++) {
        delete this->_colorSchemes.at(i);
    }
    this->_colorSchemes.clear();

    qDebug() << "Destroy instance";
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "Close window";

    this->saveTree();
    this->saveNotes();

    QMainWindow::closeEvent(event);
}

bool MainWindow::saveTree()
{
    qDebug() << "Saving tree to file";

    // Update library with user data
    QJsonObject library = this->_library;
    QTreeWidgetItem *root = this->_ui->notesTree->invisibleRootItem();
    QJsonArray notes;
    for (int i = 0; i < root->childCount(); i++) {
        NoteTreeItem *note = dynamic_cast<NoteTreeItem*>(root->child(i));
        if (!note) {
            qCritical("Could not convert tree item to note tree item");
            return false;
        }

        notes.append(note->toJson());
    }
    library.insert("notes", notes);

    // Create new json document
    this->_library = library;
    QJsonDocument doc;
    doc.setObject(this->_library);

    return this->save(doc);
}

bool MainWindow::saveNotes()
{
    qDebug() << "Saving notes to file";

    // Update library with user data
    for (int i = 0; i < this->_ui->editorTabWidget->count(); i++) {
        Editor *editor = dynamic_cast<Editor*>(this->_ui->editorTabWidget->widget(i));
        if (!editor) {
            qCritical("Could not convert tab to editor");
            return false;
        }

        if (editor->isDirty())
            editor->save();
    }

    return this->saveTree();
}

bool MainWindow::save(QJsonDocument doc)
{
    // Save json document to file
    QFile libraryFile(this->_path);
    if (!libraryFile.open(QIODevice::WriteOnly)) {
        qCritical("%s", QString("Could not save to file %1: %2").arg(this->_path).arg(libraryFile.errorString()).toUtf8().constData());
        return false;
    }

    if (libraryFile.write(doc.toJson(), doc.toJson().length()) <= -1) {
        qCritical("%s", QString("Could not save to file %1: %2").arg(this->_path).arg(libraryFile.errorString()).toUtf8().constData());
        return false;
    }

    return true;
}

bool MainWindow::load()
{
    qDebug() << "Loading data";

    this->setWindowTitle(QString("%1 - Notes").arg(this->_library.value("name").toString("")));

    QJsonArray notes;
    if (this->_library.value("notes").isArray()) {
        notes = this->_library.value("notes").toArray();
    } else {
        qWarning("'notes' is not an array!");
        this->_library.insert("notes", QJsonArray());
    }

    this->loadNotes(notes);

    return true;
}

void MainWindow::loadNotes(QJsonArray notes)
{
    qDebug() << "Loading notes";

    QTreeWidgetItem *root = this->_ui->notesTree->invisibleRootItem();

    for (int i = 0; i < notes.count(); i++) {
        QJsonObject child;
        if (notes.at(i).isObject())
            child = notes.at(i).toObject();

        NoteTreeItem::ItemType type;
        if (child.value("category").toBool(false))
            type = NoteTreeItem::CategoryType;
        else
            type = NoteTreeItem::NoteType;

        root->addChild(new NoteTreeItem(child, type));
    }
}

void MainWindow::addTreeItem(bool isCategory)
{
    QString defaultTitle;
    NoteTreeItem::ItemType type;

    if (isCategory) {
        qDebug() << "Add new category";
        defaultTitle = tr("Category");
        type = NoteTreeItem::CategoryType;
    } else {
        qDebug() << "Add new note";
        defaultTitle = tr("Note");
        type = NoteTreeItem::NoteType;
    }

    NoteTreeItem *category = new NoteTreeItem(defaultTitle, type);
    QTreeWidgetItem *parent;
    if (this->_ui->notesTree->selectedItems().count() > 0) {
        parent = this->_ui->notesTree->selectedItems().first();

        if (parent->type() != NoteTreeItem::CategoryType)
            parent = parent->parent();
    } else {
        parent = this->_ui->notesTree->invisibleRootItem();
    }

    parent->addChild(category);
    parent->setExpanded(true);

    this->saveTree();
}

Editor* MainWindow::currentEditor()
{
    if (this->_ui->editorTabWidget->count() == 0)
        return Q_NULLPTR;

    return dynamic_cast<Editor*>(this->_ui->editorTabWidget->currentWidget());
}

void MainWindow::latexToHtml(Editor *editor)
{
    qDebug() << "Convert latex to html";

    if (this->_convertProcess->state() != QProcess::NotRunning) {
        qWarning("Conversion currently running");
        return;
    }

    this->_ui->convertLogEdit->clear();

    // Create temp path
    QDir tempPath(QString("%1/%2").arg(QDir::tempPath()).arg("notes"));
    if (!tempPath.exists() && !tempPath.mkdir(tempPath.absolutePath())) {
        qCritical("%s", QString("Temporary directory does not exists and could not be created: %1!").arg(tempPath.absolutePath()).toUtf8().constData());
        return;
    }

    // Write latex into temp file
    QFile tempFile(QString("%1/%2.tex").arg(tempPath.absolutePath()).arg(QUuid::createUuid().toString().mid(1, 36)));
    if (!tempFile.open(QIODevice::WriteOnly)) {
        qCritical("%s", QString("Could not create temp file: %1!").arg(tempFile.errorString()).toUtf8().constData());
        return;
    }
    tempFile.write(editor->toPlainText().toUtf8().constData());
    tempFile.close();

    // Set process arguments
    QString program;
    QStringList arguments;
    arguments << tempFile.fileName();// << "\"html,mathml\"";
    qDebug() << arguments;

#ifdef Q_OS_WIN
    qDebug() << "Converting on Windows";
    program = "htlatex.exe";
#else
    qCritical("Unknown OS");
    return;
#endif

    this->_convertProcess->setProgram(program);
    this->_convertProcess->setArguments(arguments);
    this->_convertProcess->setWorkingDirectory(tempPath.absolutePath());
    this->_convertProcess->start();

    qDebug() << "Started conversion";
}

void MainWindow::previewContents(QUrl url)
{
    qDebug() << "Preview" << url;
    this->_ui->webView->load(url);
}
