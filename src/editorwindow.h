#pragma once

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QString>
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
    void loadPorts();

    QString fileName;

    //Menu
    void setupFileMenu();
    void setupHelpMenu();

private slots:
    void on_actionOpen();
    void on_actionSave();
    void on_actionSaveAs();
};
