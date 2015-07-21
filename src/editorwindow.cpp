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
#include <QVariant>
#include "highlighter.h"
#include <QMap>
#include <utility>

QMap<QString, QSerialPortInfo> ports;

EditorWindow::EditorWindow(QWidget *parent) :
    QMainWindow(parent),
    fileName(""),
    codeEditor(new CodeEditor),
    console(new QPlainTextEdit),
    ui(new Ui::EditorWindow)
{
    ui->setupUi(this);

    setupFileMenu();
    setupToolsMenu();
    setupHelpMenu();

    QSplitter* mEditorSplitter = new QSplitter();
    mEditorSplitter->setOrientation(Qt::Vertical);

    Highlighter *highlighter = new Highlighter(codeEditor->document());

    console->setStyleSheet("background-color: black; color: white; font: 12pt 'Courier';");
    console->appendPlainText("Initializing...\nOK!");
    console->setEnabled(false);

    mEditorSplitter->addWidget(codeEditor);
    mEditorSplitter->addWidget(console);

    mEditorSplitter->setStretchFactor(0, 4);
    mEditorSplitter->setStretchFactor(1, 1);

    setCentralWidget(mEditorSplitter);
    //loadPorts();

}

void EditorWindow::loadPorts()
{
    portsMenu->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QAction *action = portsMenu->addAction("" + info.portName());
        //info.QSerialPortInfo;
        ports[info.portName()] = info;
        action->setData(info.portName());
        action->setCheckable(true);
        if(info.portName() == port) action->setChecked(true);
    }
}

void EditorWindow::selectPort(QAction *action)
{
    port = action->data().toString();
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

        codeEditor->clear();
        codeEditor->appendPlainText(in.readAll());
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
            codeEditor->toPlainText();
            stream.flush();
            file.close();
        }
    } else {
        on_actionSaveAs();
    }
}

void EditorWindow::on_actionSaveAs()
{
    fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString(),
            tr("C++ Files (*.cpp *.h)"));

    on_actionSave();
}

void EditorWindow::on_actionNew()
{
    codeEditor->clear();
    fileName = "";
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

void EditorWindow::setupToolsMenu()
{
    QMenu *fileMenu = new QMenu(tr("&Tools"), this);
    ui->menuBar->addMenu(fileMenu);

    fileMenu->addAction(tr("&Build"), this, SLOT(on_actionBuild()));
    fileMenu->addSeparator();
    portsMenu = fileMenu->addMenu(tr("Port"));

    connect(portsMenu, SIGNAL(aboutToShow()), this, SLOT(loadPorts()));
    connect(portsMenu, SIGNAL(triggered(QAction*)), this, SLOT(selectPort(QAction*)));
}

void EditorWindow::on_actionBuild()
{

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
