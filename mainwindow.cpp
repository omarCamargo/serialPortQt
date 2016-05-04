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
    connect(manager, SIGNAL(messageReady(QByteArray)),this,SLOT(processMesageFromSerial(QByteArray)));
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

void MainWindow::exportCurvesToFile()
{
    QTextStream out;

    if( !timeSignal.isEmpty() && !firstSignal.isEmpty() && !secondSignal.isEmpty() ){
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        tr("Guardar Archivo"),
                                                        QString(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)),tr("Text files (*.txt)"));
        if( ! fileName.isEmpty() ){
            //se dio una dirección: Código
            QFile file(fileName);
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            if(!file.isOpen()){
                qDebug()<< ":( vida gono";
            }else{
                out.setDevice(&file);
                out << "Time" << "," << "Value Var1" << "," << "Value Var2" <<","  << " frequency = :(" << endl;
                for(int i =0; i < firstSignal.count();i++){
                    out << QString::number(timeSignal.at(i))<<","<<QString::number(firstSignal.at(i))<<","<<QString::number(secondSignal.at(i)) << endl;
                }
                file.flush();
                file.close();
            }
         }
    }else{
        QMessageBox::warning(this,"Advertencia","No hay señales para exportar");
    }


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

void MainWindow::processMesageFromSerial(QByteArray arr)
{
    int var1=0;
    int var2=0;
    if(arr.contains('P')){
        if(arr.length() == 6){
            if(ui->tabWidget->currentIndex() == 0){
                        ui->tabWidget->setCurrentIndex(1);
            }
            var1=QString(arr.left(3)).toInt();
            var2=QString(arr.right(3)).toInt();
            firstSignal.append(double(var1));
            secondSignal.append(double(var2));
            timeSignal.append(samplesCounter++);
            firstSignalCurve->setSamples(timeSignal,firstSignal);
            secondSignalCurve->setSamples(timeSignal,secondSignal);
            ui->widgetToPlot->replot();
        }else if(arr.length() == 7){
            if(arr.at(0)== 'w'){
                ui->lblPressures->setText( QString::number(QString(arr.mid(1,3).toInt()))+" / "+QString::number(QString(arr.mid(4,3).toInt()))+" ("+QString::number(QString(arr.right(3).toInt()))+")" );
            }
        }
    }else{
        qDebug()<<arr;
        ui->lbldatosRecibidos->setText(arr);
    }

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
    //clearQWtPlotWidget();
    //ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::on_toolButton_3_clicked()
{
    clearQWtPlotWidget();
}

void MainWindow::on_toolButton_2_clicked()
{
    exportCurvesToFile();
}
