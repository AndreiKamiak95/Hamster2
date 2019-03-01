#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    configFile = new QFile("config.csv");
    settingsList = new QList<Settings>();
    dataList = new QList<QByteArray>();
    nameList = new QList<QByteArray>();
    nameListint64 = new QList<quint64>();
    timer1 = new QTimer();
    reset = "T19EF00FE808FFFFFFFFFFFFFF\r";

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

    connect(timer1, SIGNAL(timeout()), this, SLOT(StopTimer1()));
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
    selectedSettings = new Settings(settingsList->at(ui->cmbxFile->currentIndex()));
    QFile *file = new QFile("./files/" + selectedSettings->name_file);

    if(!file->open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "ERROR", "File not found");
        return;
    }
    while (!file->atEnd())
    {
        char buf[8];
        file->read(buf, 8);
        QByteArray msg;
        msg[0] = 't';
        msg[1] = '2';
        msg[2] = toASCII((selectedSettings->id_bootloader&0xF0)>>4);
        msg[3] = toASCII (selectedSettings->id_bootloader&0x0F);
        msg[4] = '8';
        msg[5]  = toASCII((buf[0]&0xF0)>>4);
        msg[6]  =  toASCII(buf[0]&0x0F);
        msg[7]  = toASCII((buf[1]&0xF0)>>4);
        msg[8]  =  toASCII(buf[1]&0x0F);
        msg[9]  = toASCII((buf[2]&0xF0)>>4);
        msg[10] =  toASCII(buf[2]&0x0F);
        msg[11] = toASCII((buf[3]&0xF0)>>4);
        msg[12] =  toASCII(buf[3]&0x0F);
        msg[13] = toASCII((buf[4]&0xF0)>>4);
        msg[14] =  toASCII(buf[4]&0x0F);
        msg[15] = toASCII((buf[5]&0xF0)>>4);
        msg[16] =  toASCII(buf[5]&0x0F);
        msg[17] = toASCII((buf[6]&0xF0)>>4);
        msg[18] =  toASCII(buf[6]&0x0F);
        msg[19] = toASCII((buf[7]&0xF0)>>4);
        msg[20] =  toASCII(buf[7]&0x0F);
        msg[21] = 0x0D;

        dataList->append(msg);
    }
    iterator = 0;
    count_data_msg = dataList->length();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(dataList->length() - 1);
    ui->progressBar->setValue(iterator);

    QByteArray req_adr_clame("T18EEFEFF8FFFFFFFFFFFFFFFF\r");
    port->write(req_adr_clame, 27);
}

void MainWindow::readData()
{
    QByteArray receivedData = port->readAll();

    for(int i = 0; i < receivedData.length(); i++)
    {
        //29bit id
        if(receivedData[i] == 'T')
        {
            QByteArray one_msg29;
            one_msg29 = receivedData.mid(i, 27-1);

            if(!one_msg29.indexOf("18EEFF"))
            {
                timer1->start(250);
                nameList->append(one_msg29);
                quint64 byte8;
                quint64 byte[8];
                byte[0] = toNUM(one_msg29[10])<<4;
                byte[0] |= toNUM(one_msg29[11]);
                byte[1] = toNUM(one_msg29[12])<<4;
                byte[1] |= toNUM(one_msg29[13]);
                byte[2] = toNUM(one_msg29[14])<<4;
                byte[2] |= toNUM(one_msg29[15]);
                byte[3] = toNUM(one_msg29[16])<<4;
                byte[3] |= toNUM(one_msg29[17]);
                byte[4] = toNUM(one_msg29[18])<<4;
                byte[4] |= toNUM(one_msg29[19]);
                byte[5] = toNUM(one_msg29[20])<<4;
                byte[5] |= toNUM(one_msg29[21]);
                byte[6] = toNUM(one_msg29[22])<<4;
                byte[6] |= toNUM(one_msg29[23]);
                byte[7] = toNUM(one_msg29[24])<<4;
                byte[7] |= toNUM(one_msg29[25]);

                byte[1] = byte[1]<<8;
                byte[2] = byte[2]<<16;
                byte[3] = byte[3]<<24;
                byte[4] = byte[4]<<32;
                byte[5] = byte[5]<<40;
                byte[6] = byte[6]<<48;
                byte[7] = byte[7]<<56;

                byte8 = byte[0]|byte[1]|byte[2]|byte[3]|byte[4]|byte[5]|byte[6]|byte[7];

                nameListint64->append(byte8);
            }
        }
        else if(receivedData[i] == 't') //11bit id
        {
            QByteArray one_msg11, id11;
            one_msg11 = receivedData.mid(i, 21-1);
            id11 = one_msg11.mid(1,3-1);

            if(!id11.indexOf("700"))
            {
                QByteArray send_msg("t600801FFFFFF00000000\r");
                send_msg[2]  = toASCII((selectedSettings->id_bootloader&0xF0)>>4);
                send_msg[3]  = toASCII(selectedSettings->id_bootloader&0x0F);
                send_msg[13] = toASCII((count_data_msg&0x000000F0)>>4);
                send_msg[14] = toASCII( count_data_msg&0x0000000F);
                send_msg[15] = toASCII((count_data_msg&0x0000F000)>>12);
                send_msg[16] = toASCII((count_data_msg&0x00000F00)>>8);
                send_msg[17] = toASCII((count_data_msg&0x00F00000)>>20);
                send_msg[18] = toASCII((count_data_msg&0x000F0000)>>16);
                send_msg[19] = toASCII((count_data_msg&0xF0000000)>>28);
                send_msg[20] = toASCII((count_data_msg&0x0F000000)>>24);

                port->write(send_msg, 21);
            }
            else if(!id11.indexOf("300"))
            {
                if(iterator < dataList->length())
                {
                    port->write(dataList->at(iterator), 21);
                    ui->progressBar->setValue(iterator);
                    iterator++;
                }
                else
                {
                    QByteArray msg_end("t600802FFFFFFFFFFFFFF\r");
                    msg_end[2] = toASCII((selectedSettings->id_bootloader&0xF0)>>4);
                    msg_end[3] = toASCII( selectedSettings->id_bootloader&0x0F);
                }
            }
        }
    }
}

void MainWindow::StopTimer1()
{
    timer1->stop();
    quint64 buf1, buf2;
    buf1 = selectedSettings->name_unit & selectedSettings->mask;
    int num_find;

    for(int i = 0; i < nameListint64->length(); i++)
    {
        buf2 = nameListint64->at(i) & selectedSettings->mask;
        if(buf1 == buf2)
        {
            num_find = i;
            DA = toNUM(nameList->at(i)[7])<<4;
            DA |= toNUM(nameList->at(i)[8]);
            break;
        }
    }
    reset[5] = nameList->at(num_find)[7];
    reset[6] = nameList->at(num_find)[8];

    port->write(reset);
}

quint8 MainWindow::toASCII(quint8 num)
{
    quint8 buf = 0x00;
    if((num >= 0) && (num <= 9))
        buf = num + '0';
    else if((num >= 10) && (num <= 15))
        buf = num + 55;

    return buf;
}

quint8 MainWindow::toNUM(quint8 symbol)
{
    quint8 buf = 0x00;
    if((symbol >= '0') && (symbol <= '9'))
        buf = symbol-'0';
    else if((symbol >= 'A') && (symbol <= 'F'))
        buf = symbol-55;

    return buf;
}
