#include "serialportmanager.h"

SerialPortManager::SerialPortManager(QObject *parent) : QObject(parent)
{

}

QStringList SerialPortManager::searchAvailablePorts()
{
    QStringList lst;
    currentSerialPorts =  QSerialPortInfo::availablePorts();
    foreach ( const QSerialPortInfo &info, currentSerialPorts){
       lst.append(info.portName());
    }

    return lst;
}

QString SerialPortManager::getSelectedPortName()
{
    return selectedPort->portName();
}

QString SerialPortManager::getSelectedPortDescription()
{
    return selectedPort->description();
}

QString SerialPortManager::getSelectedPortManufacturer()
{
    return selectedPort->manufacturer();
}

qint16 SerialPortManager::getSelectedPortVID()
{
    return selectedPort->vendorIdentifier();
}

qint16 SerialPortManager::getSelectedPortPID()
{
    return selectedPort->productIdentifier();
}
QSerialPortInfo *SerialPortManager::getSelectedPort() const
{
    return selectedPort;
}

void SerialPortManager::setSelectedPort(QString *Name)
{
    selectedPort = new QSerialPortInfo(*Name);
}


