#include "src/monitordialog.h"
#include "ui_monitordialog.h"

#include <QSerialPortInfo>
#include <QScrollBar>
#include <QByteArray>

MonitorDialog::MonitorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MonitorDialog)
{
    ui->setupUi(this);

    outputText = ui->outputText;
    inputText = ui->inputText;
    sendButton = ui->sendButton;


    serial = new QSerialPort(this);

    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

void MonitorDialog::setPort(QSerialPortInfo portInfo)
{
    serial->setPort(portInfo);
    serial->open(QIODevice::ReadWrite);
    serial->setBaudRate(9600);
}

MonitorDialog::~MonitorDialog()
{
    delete ui;
}

void MonitorDialog::readData()
{
    QByteArray data = serial->readAll();
    outputText->appendPlainText(data);
    outputText->verticalScrollBar()->setValue(outputText->verticalScrollBar()->maximum());
}
