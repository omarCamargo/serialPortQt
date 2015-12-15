#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    conectedtoASerialPort=false;

   if( Deviceserialport == NULL){
        qDebug() << "no existe objeto";

    }else{
        qDebug() << " ninguno de los anteriores";
    }
   qDebug() << "Hola Con git";





}

MainWindow::~MainWindow()
{
    delete ui;

    if(Deviceserialport != NULL && Deviceserialport->isOpen()){
        Deviceserialport->close();
    }



}

void MainWindow::on_btnBuscar_clicked()
{
    ui->MylistWidget->clear();

    serialList =  QSerialPortInfo::availablePorts();
    foreach ( const QSerialPortInfo &info, serialList){
        ui->MylistWidget->addItem(info.portName());
    }



}

void MainWindow::on_MylistWidget_doubleClicked(const QModelIndex &index)
{

    serialPortSelectedName=index.data().toString();
    QSerialPortInfo *port= new QSerialPortInfo(serialPortSelectedName);//ojo esto es absolutamente necesario puesto que index es una variable muy general, se debe obtener el data y luego convertirlo a qString
    serialPortSelectedDescription=port->description();

    ui->lblSerialName->setText(port->description());
    ui->lblSerialManufacter->setText(port->manufacturer());
    ui->btnConectar->setEnabled(true);
    ui->lbl_PID->setText( QString::number(port->productIdentifier()) );
    ui->lbl_VID->setText( QString::number(port->vendorIdentifier()) );

}

void MainWindow::on_btnConectar_clicked()
{
   if(conectedtoASerialPort){

       conectedtoASerialPort=false;
       ui->btnConectar->setText("Conectar");
       ui->btnConectar->setEnabled(false);
       ui->lblSerialConected->setText("Sin Conexión");
       ui->edtxEnviar->setEnabled(false);
       ui->btnEnviar->setEnabled(false);
       disconnect(Deviceserialport, SIGNAL(readyRead()), this, SLOT(retrieveDataFromSerialPort()));
       ui->edtxEnviar->setText("");
       ui->lbldatosRecibidos->setText("------------------");
       ui->lblSerialName->setText("------------");
       ui->lblSerialManufacter->setText("------------");
       Deviceserialport->close();




   }else{
       conectedtoASerialPort=true;
       ui->btnConectar->setText("Desconectar");
       ui->lblSerialConected->setText(serialPortSelectedDescription);
       Deviceserialport = new QSerialPort(serialPortSelectedName,this);
       qDebug() << Deviceserialport;
       Deviceserialport->setBaudRate(QSerialPort::Baud9600);
       Deviceserialport->setDataBits(QSerialPort::Data8);
       Deviceserialport->setParity(QSerialPort::NoParity);
       Deviceserialport->setStopBits(QSerialPort::OneStop);
       Deviceserialport->setFlowControl(QSerialPort::NoFlowControl);
       bool result=Deviceserialport->open(QIODevice::ReadWrite);
      if(result){
          ui->edtxEnviar->setEnabled(true);
          ui->btnEnviar->setEnabled(true);
          connect(Deviceserialport, SIGNAL(readyRead()), this, SLOT(retrieveDataFromSerialPort()));
          connect(Deviceserialport,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(getErrorFromSerial(QSerialPort::SerialPortError)));
          if( Deviceserialport != NULL){
              qDebug() << " existe objeto";

          }else{
              qDebug() << " no existe objeto";

          }


      }//cierre if que pregunta por el resultado de intentar abrir el puerto
   }
}

void MainWindow::on_btnEnviar_clicked()
{

    Deviceserialport->write(ui->edtxEnviar->text().toStdString().c_str());

}


void  MainWindow::retrieveDataFromSerialPort()
{

    quint8 valuint8;
    quint16 valuint16;
    QString messageReceived;

    QByteArray mbyteArray = Deviceserialport->readAll();
    qDebug()<< mbyteArray;
    if(mbyteArray.at(0) == 'A' || mbyteArray.at(0) == 'P' ||  mbyteArray.at(0) == 'C' || mbyteArray.at(0) == '2' ){

        if(mbyteArray.at(0)=='A'){
            valuint16 = (mbyteArray.at(1)<<8)|mbyteArray.at(2);
            messageReceived = "Valor actual de Presión: " +QString::number(valuint16)+" mmHg";
        }else if(mbyteArray.at(0)=='P'){
            valuint16 = (mbyteArray.at(1)<<8)|mbyteArray.at(2);
            messageReceived = "Valor actual de la señal pulsatil: "+QString::number(valuint16);
        }else if(mbyteArray.at(0)=='C'){
            valuint16 = (mbyteArray.at(1)<<8)|mbyteArray.at(2);
            messageReceived= "Valor del offset: "+QString::number(valuint16);
        }else if(mbyteArray.at(0)=='2'){
            valuint16 = (mbyteArray.at(1)<<8)|mbyteArray.at(2);
            messageReceived = "Valor actual depresión: "+QString::number(valuint16)+" mmHg<br>";
            valuint16 = (mbyteArray.at(3)<<8)|mbyteArray.at(4);
            messageReceived = messageReceived +"Valor actual de la señal pulsatil: "+QString::number(valuint16);

        }



    }else if(mbyteArray.at(0)=='p' || mbyteArray.at(0)=='r'  || mbyteArray.at(0)=='s'|| mbyteArray.at(0)=='t'){

        valuint8 =  (quint8) mbyteArray.at(1);
        //qDebug() << valuint8
        messageReceived = "potenciometro al nivel: "+ QString::number(valuint8);

    }else{
        messageReceived=QString(mbyteArray);
    }


    ui->lbldatosRecibidos->setText(messageReceived);


}

void MainWindow::getErrorFromSerial(QSerialPort::SerialPortError err )
{
    if(err == QSerialPort::ResourceError){
        QMessageBox::critical(this, tr("Critical Error"), "conexión perdida"/*Deviceserialport->errorString()*/);
        Deviceserialport->close();
        conectedtoASerialPort=false;
        ui->btnConectar->setText("Conectar");
        ui->btnConectar->setEnabled(false);
        ui->btnEnviar->setEnabled(false);
        ui->lblSerialConected->setText("Sin Conexión");
        ui->edtxEnviar->setEnabled(false);
        disconnect(Deviceserialport, SIGNAL(readyRead()), this, SLOT(retrieveDataFromSerialPort()));
        ui->edtxEnviar->setText("");
        ui->lbldatosRecibidos->setText("------------------");
        ui->lblSerialName->setText("------------");
        ui->lblSerialManufacter->setText("------------");
        ui->MylistWidget->clear();

    }

}
