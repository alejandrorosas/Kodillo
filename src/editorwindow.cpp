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
#include <QCompleter>
#include <QPixmap>
#include <QSize>
#include "monitordialog.h"

QMap<QString, QSerialPortInfo> ports;

EditorWindow::EditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditorWindow),
    fileName(""),
    console(new QPlainTextEdit),
    codeEditor(new CodeEditor)
{
    ui->setupUi(this);

    setupFileMenu();
    setupEditMenu();
    setupToolsMenu();
    setupHelpMenu();
    setupToolbar();

    QSplitter* mEditorSplitter = new QSplitter();
    mEditorSplitter->setOrientation(Qt::Vertical);

    new Highlighter(codeEditor->document());

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
        save();
    } else {
        on_actionSaveAs();
    }
}

void EditorWindow::save()
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        // error message
    } else {
        QTextStream stream(&file);
        stream << codeEditor->toPlainText();
        stream.flush();
        file.close();
    }
}

void EditorWindow::on_actionSaveAs()
{
    fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString(),
            tr("C++ Files (*.cpp *.h)"));
    if (fileName != "")
    {
        save();
    }
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

void EditorWindow::setupEditMenu()
{
    QMenu *editMenu = new QMenu(tr("&Edit"), this);
    ui->menuBar->addMenu(editMenu);

    //codeEditor->undo();
    editMenu->addAction(tr("Undo"),codeEditor,SLOT(undo()),QKeySequence::Undo);
    editMenu->addAction(tr("Redo"),codeEditor,SLOT(redo()),QKeySequence::Redo);
    editMenu->addSeparator();
    editMenu->addAction(tr("Cut"),codeEditor,SLOT(cut()),QKeySequence::Cut);
    editMenu->addAction(tr("Copy"),codeEditor,SLOT(copy()),QKeySequence::Copy);
    editMenu->addAction(tr("Paste"),codeEditor,SLOT(paste()),QKeySequence::Paste);
    editMenu->addAction(tr("Select All"),codeEditor,SLOT(selectAll()),QKeySequence::SelectAll);
    editMenu->addSeparator();
    editMenu->addAction(tr("Auto Indent"),codeEditor,SLOT(autoIndentSlot()),tr("Ctrl+T"));
    editMenu->addAction(tr("Increase Indent Selection"),codeEditor,SLOT(increaseIndentSlot()),tr("Tab"));
    editMenu->addAction(tr("Decrease Indent Selection"), codeEditor, SLOT(decreaseIndentSlot()),tr("Backtab"));

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

    helpMenu->addAction(tr("&About"));
}

EditorWindow::~EditorWindow()
{
    delete ui;
}

void EditorWindow::setupToolbar()
{
    QPixmap buildIcon = QPixmap(":/images/toolbar/build.png");
    QPixmap runIcon = QPixmap(":/images/toolbar/run.png");
    QPixmap serialIcon = QPixmap(":/images/toolbar/serial.png");

    buildToolbar = new QAction(buildIcon, "Build", this);
    runToolbar = new QAction(runIcon, "Run", this);
    serialToolbar = new QAction(serialIcon, "Start Serial", this);


    QWidget *spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidget->setVisible(true);

    buildToolbar->setEnabled(true);
    runToolbar->setEnabled(false);

    QToolBar *toolbar = addToolBar("Main Toolbar");
    toolbar->setIconSize( QSize( 24, 24 ) );
    toolbar->setMovable(false);
    toolbar->addAction(buildToolbar);
    toolbar->addAction(runToolbar);
    toolbar->addWidget(spacerWidget);
    toolbar->addAction(serialToolbar);

    connect(buildToolbar, SIGNAL(triggered()), this, SLOT(toolbarBuild()));
    connect(runToolbar, SIGNAL(triggered()), this, SLOT(toolbarRun()));
    connect(serialToolbar, SIGNAL(triggered()), this, SLOT(toolbarShowMonitor()));
}

void EditorWindow::toolbarBuild()
{

}

void EditorWindow::toolbarRun()
{

}

void EditorWindow::toolbarShowMonitor()
{
    MonitorDialog *monitor = new MonitorDialog();
    monitor->show();
}
