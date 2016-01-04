#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class SerialPortManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortManager(QObject *parent = 0);

signals:

public slots:
private:
    QSerialPort serialPort;
    QList<QSerialPortInfo> currentSerialPorts;
};

#endif // SERIALPORTMANAGER_H
