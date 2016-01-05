#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    manager = new SerialPortManager(this);
    connect(manager, SIGNAL(messageReady(QString)),this,SLOT(processMesageFromSerial(QString)));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnBuscar_clicked()
{
    ui->MylistWidget->clear();
    ui->MylistWidget->addItems(manager->searchAvailablePorts());
}

void MainWindow::on_MylistWidget_doubleClicked(const QModelIndex &index)
{
    QString s = index.data().toString();
    manager->setSelectedPort(&s);
    ui->lblSerialDescription->setText(manager->getSelectedPortDescription());
    ui->lblSerialManufacter->setText(manager->getSelectedPortManufacturer());
    ui->lbl_PID->setText(QString::number(manager->getSelectedPortPID()));
    ui->lbl_VID->setText(QString::number(manager->getSelectedPortVID()));
    ui->btnConectar->setEnabled(true);

}

void MainWindow::on_btnConectar_clicked()
{

    if(!manager->isAnyPortConnected()){
       if(manager->connectSelectedPort(manager->getSelectedPortName())){
           ui->btnConectar->setText("Desconectar");
           ui->edtxEnviar->setEnabled(true);
           ui->btnEnviar->setEnabled(true);
           ui->lblSerialConected->setText(manager->getSelectedPortDescription());
       }else{
           //error en conexión
       }
    }else{
        if(manager->disconnectSelectedPort()){
            ui->btnConectar->setText("Conectar");
            ui->edtxEnviar->setEnabled(false);
            ui->btnEnviar->setEnabled(false);
            ui->lblSerialConected->setText("Sin Conexión");
        }else{
            //Error al cerrar el puerto;
        }
    }

}

void MainWindow::on_btnEnviar_clicked()
{

    manager->sendMessageToPort(ui->edtxEnviar->text());

}


void  MainWindow::retrieveDataFromSerialPort()
{


}

void MainWindow::processMesageFromSerial(QString s)
{
    qDebug() << s;
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
        ui->lblSerialDescription->setText("------------");
        ui->lblSerialManufacter->setText("------------");
        ui->MylistWidget->clear();

    }

}
