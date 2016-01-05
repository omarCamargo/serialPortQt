#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qserialport>
#include <QSerialPortInfo>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include "serialportmanager.h"

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
    void on_btnBuscar_clicked();

    void on_MylistWidget_doubleClicked(const QModelIndex &index);

    void on_btnConectar_clicked();

    void on_btnEnviar_clicked();

    void retrieveDataFromSerialPort();

    void processMesageFromSerial(QString s);

    void getErrorFromSerial(QSerialPort::SerialPortError err);

private:
    Ui::MainWindow *ui;
    SerialPortManager *manager;
    QString serialPortSelectedName;
    QString serialPortSelectedDescription;
    QSerialPort * Deviceserialport;
    bool conectedtoASerialPort;
    QList<QSerialPortInfo> serialList;





};

#endif // MAINWINDOW_H
