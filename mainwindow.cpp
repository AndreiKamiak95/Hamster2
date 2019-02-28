#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    configFile = new QFile("config.csv");
    settingsList = new QList<Settings>();

    if(!configFile->open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "ERROR", "Config file not found");
        return;
    }
    configFile->readLine();
    while (!configFile->atEnd())
    {
        Settings buf(configFile->readLine());
        settingsList->append(buf);
        ui->cmbxFile->addItem(buf.name);
    }


    portList = new QList<QSerialPortInfo>();
    *portList = QSerialPortInfo::availablePorts();
    for(int i = 0; i < portList->count(); i++)
    {
        ui->cmbxPort->addItem(portList->at(i).portName());
    }


    ui->lblMovie->setText("");
    ui->lblInfo->setText("");
    mo.setFileName(":/ico/gif/movie.gif");
    mo.setScaledSize(ui->lblMovie->size());
    ui->lblMovie->setMovie(&mo);
    mo.start();
    mo.stop();

    ui->btnDisconnect->setEnabled(false);
    ui->btnLoad->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnConnect_clicked()
{
    port = new QSerialPort(ui->cmbxPort->currentText());
    port->setBaudRate(QSerialPort::Baud115200);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);

    connect(port, SIGNAL(readyRead()), this, SLOT(readData()));

    if(port->open(QIODevice::ReadWrite) == false)
    {
        QMessageBox::critical(this, "Hamster", "Port not open");
        return;
    }

    char buff1[] = {0x43, 0x0D};
    char buff2[] = {0x53, 0x35, 0x0D};
    char buff3[] = {0x4F, 0x0D};
    port->write(buff1, 2);
    port->write(buff2, 3);
    port->write(buff3, 2);

    ui->btnConnect->setEnabled(false);
    ui->btnDisconnect->setEnabled(true);
    ui->btnLoad->setEnabled(true);
    ui->cmbxFile->setEnabled(false);
    ui->cmbxPort->setEnabled(false);
}

void MainWindow::on_btnDisconnect_clicked()
{
    char buff1[] = {0x43, 0x0D};
    port->write(buff1, 2);
    port->close();

    ui->btnConnect->setEnabled(true);
    ui->btnDisconnect->setEnabled(false);
    ui->btnLoad->setEnabled(false);
    ui->cmbxFile->setEnabled(true);
    ui->cmbxPort->setEnabled(true);
    mo.stop();
    ui->lblInfo->setText("");
}

void MainWindow::on_btnLoad_clicked()
{
    Settings selectedSettings(settingsList->at(ui->cmbSelFile->currentIndex()));
    QFile *file = new QFile("./files/" + selectedSettings.name_file);
    //дописать id

    if(!file->open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "ERROR", "File not found");
        return;
    }


}

void MainWindow::readData()
{

}
