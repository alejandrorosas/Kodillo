#pragma once

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QString>
#include <QMenu>
#include <QSerialPortInfo>
#include <QAction>
#include "codeeditor.h"

namespace Ui {
class EditorWindow;
}

class EditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditorWindow(QWidget *parent = 0);
    ~EditorWindow();

private:

    Ui::EditorWindow *ui;

    QString fileName;
    QString port;

    QPlainTextEdit *console;
    CodeEditor *codeEditor;

    QMenu *portsMenu;

    //Menu
    void setupFileMenu();
    void setupHelpMenu();
    void setupToolsMenu();

private slots:
    void on_actionOpen();
    void on_actionSave();
    void on_actionSaveAs();
    void on_actionBuild();
    void on_actionNew();
    void loadPorts();
    void selectPort(QAction *action);
};
