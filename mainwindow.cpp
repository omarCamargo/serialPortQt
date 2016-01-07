#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    samplesCounter = 0;
    setupQWtPlotWidget();
    manager = new SerialPortManager(this);
    connect(manager, SIGNAL(messageReady(QString)),this,SLOT(processMesageFromSerial(QString)));
    on_btnBuscar_clicked();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupQWtPlotWidget()
{
    firstSignalCurve = new QwtPlotCurve();
    firstSignalCurve->setTitle( "First" );
    firstSignalCurve->setPen( Qt::red, 3 );
    firstSignalCurve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    firstSignalCurve->attach(ui->widgetToPlot);

    secondSignalCurve = new QwtPlotCurve();
    secondSignalCurve->setTitle( "First" );
    secondSignalCurve->setPen( Qt::blue, 3 );
    secondSignalCurve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    secondSignalCurve->attach(ui->widgetToPlot);

    ui->widgetToPlot->setCanvasBackground(QColor(Qt::white));

}

void MainWindow::clearQWtPlotWidget()
{
    firstSignal.clear();
    secondSignal.clear();
    timeSignal.clear();
    samplesCounter = 0;
    firstSignalCurve->setSamples(timeSignal,firstSignal);
    secondSignalCurve->setSamples(timeSignal,secondSignal);
    ui->widgetToPlot->replot();
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
    if(s.length() == 7){
        qDebug() << s;
        manager->convertTwoBytestoUint16(s.at(0),s.at(1));
    }
//    if(s.contains(',')){
//        QStringList list = s.split(',',QString::SkipEmptyParts);
//        if(ui->tabWidget->currentIndex() == 0){
//            ui->tabWidget->setCurrentIndex(1);
//        }
//        firstSignal.append(list.at(0).toDouble());
//        secondSignal.append(list.at(1).toDouble());
//        timeSignal.append(samplesCounter++);
//        firstSignalCurve->setSamples(timeSignal,firstSignal);
//        secondSignalCurve->setSamples(timeSignal,secondSignal);
//        ui->widgetToPlot->replot();
//    }else{
//        ui->lbldatosRecibidos->setText(s);
//    }

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



void MainWindow::on_toolButton_clicked()
{
    manager->sendMessageToPort("S");
    clearQWtPlotWidget();
    ui->tabWidget->setCurrentIndex(0);
}
