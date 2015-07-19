#include "editorwindow.h"
#include "ui_editorwindow.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QMenu>
#include "highlighter.h"

EditorWindow::EditorWindow(QWidget *parent) :
    QMainWindow(parent),
    fileName(""),
    ui(new Ui::EditorWindow)
{
    ui->setupUi(this);

    setupFileMenu();
    setupHelpMenu();

    QSplitter* mEditorSplitter = new QSplitter();
    mEditorSplitter->setOrientation(Qt::Vertical);

    CodeEditor *codeEditor = new CodeEditor();
    codeEditor->setObjectName("codeEditor");

    Highlighter *highlighter = new Highlighter(codeEditor->document());

    mEditorSplitter->addWidget(codeEditor);

    QPlainTextEdit* console = new QPlainTextEdit();
    mEditorSplitter->addWidget(console);

    setCentralWidget(mEditorSplitter);
    //loadPorts();

}

void EditorWindow::loadPorts()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
           //ui->actionPort->menu()->addAction("" + info.portName());
        //console->appendPlainText("" + info.portName());
    }
}

void EditorWindow::on_actionOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
            tr("Kodillo Files (*.cpp)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }

        this->fileName = fileName;

        QTextStream in(&file);

        findChild<CodeEditor*>("codeEditor")->clear();
        findChild<CodeEditor*>("codeEditor")->appendPlainText(in.readAll());
        file.close();
    }
}

void EditorWindow::on_actionSave()
{
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            // error message
        } else {
            QTextStream stream(&file);
            stream << findChild<CodeEditor*>("codeEditor")->toPlainText();
            stream.flush();
            file.close();
        }
    }
}

void EditorWindow::on_actionSaveAs()
{
    fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString(),
            tr("C++ Files (*.cpp *.h)"));

    on_actionSave();
}

void EditorWindow::setupFileMenu()
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    ui->menuBar->addMenu(fileMenu);

    fileMenu->addAction(tr("&New"), this, SLOT(on_actionNew()), QKeySequence::New);
    fileMenu->addAction(tr("&Open..."), this, SLOT(on_actionOpen()), QKeySequence::Open);
    fileMenu->addAction(tr("&Save..."), this, SLOT(on_actionSave()), QKeySequence::Save);
    fileMenu->addAction(tr("&Save as..."), this, SLOT(on_actionSaveAs()), QKeySequence::SaveAs);
    fileMenu->addAction(tr("E&xit"), qApp, SLOT(quit()), QKeySequence::Quit);
}

void EditorWindow::setupHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    ui->menuBar->addMenu(helpMenu);

    helpMenu->addAction(tr("&About"), this, SLOT());
}

EditorWindow::~EditorWindow()
{
    delete ui;
}
