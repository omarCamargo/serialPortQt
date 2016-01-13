#include "serialportmanager.h"

SerialPortManager::SerialPortManager(QObject *parent) : QObject(parent)
{
    portConnected = false;

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

bool SerialPortManager::isAnyPortConnected()
{
    return getPortConnected();
}

bool SerialPortManager::connectSelectedPort(QString portName)
{
    serialPort = new QSerialPort(portName,this);
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    if(serialPort->open(QIODevice::ReadWrite)){
        connect(serialPort, SIGNAL( readyRead() ), this, SLOT( newMessageFromSerialPortReady() ));
        connect(serialPort, SIGNAL( error(QSerialPort::SerialPortError) ), this, SLOT( getErrorFromSerial(QSerialPort::SerialPortError)) );
        setPortConnected(true);
        finalMessage.clear();

        return true;
    }else{
        return false;
    }


}

bool SerialPortManager::disconnectSelectedPort()
{
    if(serialPort != NULL && serialPort->isOpen()){
        setPortConnected(false);
        serialPort->close();
        return true;
    }
    return false;
}

void SerialPortManager::sendMessageToPort(QString message)
{
    serialPort->write(message.toStdString().c_str());
}

QSerialPortInfo *SerialPortManager::getSelectedPort() const
{
    return selectedPort;
}

void SerialPortManager::setSelectedPort(QString *Name)
{
    selectedPort = new QSerialPortInfo(*Name);
}
bool SerialPortManager::getPortConnected() const
{
    return portConnected;
}

void SerialPortManager::setPortConnected(bool value)
{
    portConnected = value;
}

void SerialPortManager::newMessageFromSerialPortReady()
{

    QByteArray array = serialPort->readAll();
    emit(messageReady(array));
//    finalMessage.append(array);

//    if (finalMessage.contains('\n')){
//        emit( messageReady( finalMessage.left( finalMessage.indexOf('\n')+1 ) ) );
//        finalMessage.remove(0, finalMessage.indexOf('\n'));

//    }


}

void SerialPortManager::getErrorFromSerial(QSerialPort::SerialPortError err)
{
    qDebug() << err;
    //TO DO MAKE THIS FUNCTION
}



