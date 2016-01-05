#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>
#include <QList>

class SerialPortManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortManager(QObject *parent = 0);
    QStringList searchAvailablePorts();
    QString getSelectedPortName();
    QString getSelectedPortDescription();
    QString getSelectedPortManufacturer();
    qint16  getSelectedPortVID();
    qint16 getSelectedPortPID();


    QSerialPortInfo *getSelectedPort() const;
    void setSelectedPort(QString *Name);

signals:
    void messageReady(QString S);

public slots:

private:
    QSerialPort serialPort;
    QList<QSerialPortInfo> currentSerialPorts;
    QSerialPortInfo *selectedPort;
};

#endif // SERIALPORTMANAGER_H
