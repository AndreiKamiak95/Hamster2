#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSound>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QFile>
#include <QMessageBox>
#include <QMovie>

#include <settings.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btnConnect_clicked();
    void on_btnDisconnect_clicked();
    void on_btnLoad_clicked();
    void readData();

private:
    Ui::MainWindow *ui;
    QFile *configFile;
    QList<Settings> *settingsList;
    QList<QSerialPortInfo> *portList;
    QMovie mo;
    QSerialPort *port;
};

#endif // MAINWINDOW_H
