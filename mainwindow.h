#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSound>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QFile>
#include <QMessageBox>
#include <QMovie>
#include <QTimer>

#include <settings.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    quint8 toASCII(quint8 num);
    quint8 toNUM(quint8 symbol);
    ~MainWindow();

private slots:
    void on_btnConnect_clicked();
    void on_btnDisconnect_clicked();
    void on_btnLoad_clicked();
    void readData();
    void StopTimer1();

private:
    Ui::MainWindow *ui;
    QFile *configFile;
    QList<Settings> *settingsList;
    QList<QSerialPortInfo> *portList;
    QList<QByteArray> *dataList;
    QList<QByteArray> *nameList;
    QList<quint64> *nameListint64;
    QMovie mo;
    QSerialPort *port;
    QTimer *timer1;
    Settings *selectedSettings;

    int iterator;
    quint8 DA;
    quint32 count_data_msg;

    QByteArray reset;
};

#endif // MAINWINDOW_H
