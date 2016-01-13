#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>
#include <QDebug>
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
    bool isAnyPortConnected();
    bool connectSelectedPort( QString portName);
    bool disconnectSelectedPort();
    void sendMessageToPort(QString message);


    QSerialPortInfo *getSelectedPort() const;
    void setSelectedPort(QString *Name);

    bool getPortConnected() const;
    void setPortConnected(bool value);

signals:
    void messageReady(QByteArray arr);

public slots:
    void newMessageFromSerialPortReady();
    void getErrorFromSerial(QSerialPort::SerialPortError err);

private:
    bool portConnected;
    QSerialPort *serialPort;
    QList<QSerialPortInfo> currentSerialPorts;
    QSerialPortInfo *selectedPort;
    QByteArray finalMessage;
};

#endif // SERIALPORTMANAGER_H
