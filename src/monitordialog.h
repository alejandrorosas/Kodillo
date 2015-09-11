#ifndef MONITORDIALOG_H
#define MONITORDIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QSerialPort>

namespace Ui {
class MonitorDialog;
}

class MonitorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MonitorDialog(QWidget *parent = 0);
    ~MonitorDialog();
    void setPort(QSerialPortInfo portInfo);

private slots:
    void readData();

private:
    Ui::MonitorDialog *ui;

    QPlainTextEdit *outputText;
    QPushButton *sendButton;
    QLineEdit *inputText;

    QSerialPort *serial;
};

#endif // MONITORDIALOG_H
