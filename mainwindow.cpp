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
//    QString s = QString("qertreW123123099");
//    int i = s.indexOf("W");
//    qDebug() <<i;
    msignalProcessor = new SignalProcessor(this);
    connect(this,SIGNAL( dataToSignalProcessor(int,int) ), msignalProcessor, SLOT( processNewValueAcquired(int,int) ));
    connect(msignalProcessor, SIGNAL( diastolicAndSistolicWasFound(int,int) ), this, SLOT(displayarterialpressures(int,int))  );
    //msignalProcessor->setOffset(47);


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
                out << "Time" << "," << "Value Var1" << "," << "Value Var2" <<","  << " offset = " << msignalProcessor->getOffset()  << endl;
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
    if(ui->edtxEnviar->text().length() == 1 && ui->edtxEnviar->text().at(0) == 'I' ){
        msignalProcessor->resetVals();
    }
    manager->sendMessageToPort(ui->edtxEnviar->text());

}


void  MainWindow::retrieveDataFromSerialPort()
{


}

void MainWindow::processMesageFromSerial(QByteArray arr)
{
    int var1=0;
    int var2=0;

    if(arr.length() ==3){
        if(arr.at(0) =='M'){
            var1=QString(arr.at(1)).toInt();
            var2=QString(arr.at(2)).toInt();
            firstSignal.append(double(var1));
            secondSignal.append(double(var2));
            timeSignal.append(samplesCounter++);
            firstSignalCurve->setSamples(timeSignal,firstSignal);
            secondSignalCurve->setSamples(timeSignal,secondSignal);
            ui->widgetToPlot->replot();
        }else{
            var1=QString(arr).toInt();
            firstSignal.append(double(var1));
            timeSignal.append(samplesCounter++);
            firstSignalCurve->setSamples(timeSignal,firstSignal);
            ui->widgetToPlot->replot();
            ui->lblOffset->setText(QString::number(var1));
        }
    }else if(arr.length() == 4){
        if(arr.at(0)=='F'){
            ui->lblOffset->setText(QString::number(QString(arr.right(3)).toInt()));
            msignalProcessor->setOffset(QString(arr.right(3)).toInt());


        }else{
            if(arr.at(0) == 'P'){
                ui->lblSystRecieved->setText(QString(arr.right(3)));

            }else if(arr.at(3) == 'P'){

            }
            ui->lbldatosRecibidos->setText(QString(arr));
        }
    }else if(arr.length() == 6){
        if(ui->tabWidget->currentIndex() == 0){
            ui->tabWidget->setCurrentIndex(1);
        }

        if(arr.at(3)=='M'){
            var1=QString(arr.left(3)).toInt();
            var2=QString(arr.at(5)).toInt();
            firstSignal.append(double(var1));
            secondSignal.append(double(var2));
            timeSignal.append(samplesCounter++);
            firstSignalCurve->setSamples(timeSignal,firstSignal);
            secondSignalCurve->setSamples(timeSignal,secondSignal);
            ui->lblOffset->setText(QString::number(var1));
            ui->widgetToPlot->replot();
        }else if(arr.at(0) == 'L' || arr.at(0) == 'O' || arr.at(0) == 'R' ){
             ui->lbldatosRecibidos->setText(QString(arr));
        }else{
            var1=QString(arr.left(3)).toInt();
            var2=QString(arr.right(3)).toInt();
            firstSignal.append(double(var1));
            secondSignal.append(double(var2));
            timeSignal.append(samplesCounter++);
            //emit( dataToSignalProcessor( QString(arr.left(3)).toInt() , QString(arr.right(3)).toInt() ));
            firstSignalCurve->setSamples(timeSignal,firstSignal);
            secondSignalCurve->setSamples(timeSignal,secondSignal);
            ui->widgetToPlot->replot();
            ui->lblOffset->setText(QString::number(var1));
        }

    }else if(arr.length() == 7){
        if(arr.at(0)== 'P'){
            var1=QString(QString(arr.mid(1,3))).toInt();
            var2=QString(arr.right(3)).toInt();
            firstSignal.append(double(var1));
            secondSignal.append(double(var2));
            timeSignal.append(samplesCounter++);
            firstSignalCurve->setSamples(timeSignal,firstSignal);
            secondSignalCurve->setSamples(timeSignal,secondSignal);
            ui->widgetToPlot->replot();
        }
    }else if(arr.length() == 9){
        var1=QString(arr.left(3)).toInt();
        var2=QString(arr.mid(3,3)).toInt();
        firstSignal.append(double(var1));
        secondSignal.append(double(var2));
        timeSignal.append(samplesCounter++);
        emit( dataToSignalProcessor( QString(arr.left(3)).toInt() , QString(arr.mid(3,3)).toInt() ));
        firstSignalCurve->setSamples(timeSignal,firstSignal);
        secondSignalCurve->setSamples(timeSignal,secondSignal);
        ui->widgetToPlot->replot();
        ui->lblAdc->setText(QString::number(var1));

    }else if(arr.length() == 10){
        if(arr.at(0)== 'W'){
            qDebug() << arr;
            ui->lblPressures->setText( QString::number(QString(arr.mid(1,3)).toInt())+" / "+QString::number(QString(arr.mid(4,3)).toInt())+" ("+QString::number(QString(arr.right(3)).toInt())+")" );
            ui->lblSetPoint->setText( QString::number( QString(arr.mid(1,3)).toInt() + 20 ));
        }
    }else{
        qDebug()<<arr;
        if(arr.contains('W')){
            int wpos = arr.indexOf('W');
            ui->lblPressures->setText( QString::number(QString(arr.mid(wpos+1,3)).toInt())+" / "+QString::number(QString(arr.mid(wpos+4,3)).toInt())+" ("+QString::number(QString(arr.mid(wpos+7,3)).toInt())+")" );
            ui->lblSetPoint->setText( QString::number( QString(arr.mid(1,3)).toInt() + 20 ));
        }
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

void MainWindow::displayarterialpressures(int pdist, int psist)
{
    finishReadFlag = 0;
    if(ui->tabWidget->currentIndex() ==1){
        ui->lblPressures->setText( QString::number(psist)+"/"+ QString::number(pdist)+". " );
        QString sist = QString::number(psist);
        qDebug() << sist;
        sist = "P"+sist;
        //disconnect(this, SIGNAL(dataToSignalProcessor(int,int)), msignalProcessor,SLOT(processNewValueAcquired(int,int)));
        manager->sendMessageToPort(sist);
        delay(10);

    }else if(ui->tabWidget->currentIndex() ==2){
        ui->lblbdispPressures_2->setText(QString::number(psist)+"/"+ QString::number(pdist)+". ");
    }

}

void MainWindow::on_toolButton_4_clicked()
{
    msignalProcessor->resetVals();
    timeSignal.clear();
    firstSignal.clear();
    secondSignal.clear();
    QTextStream in;
    QString lineReaded;
    QStringList splitList;
    QString fileName = /*"/Users/GIBICUDEA/Desktop/prueba_123.txt";*/ QFileDialog::getOpenFileName(0,tr("Nombre de Archivo"),QString(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)),tr("Text files (*.txt)"));
    //qDebug() << "Nombre archivo: " << fileName;
    finishReadFlag =1;
    if( ! fileName.isEmpty() ){
        QFile file(fileName);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        if(!file.isOpen()){
            qDebug()<< ":( vida gono";
        }else{
            in.setDevice(&file);
            lineReaded = in.readLine();
            lineReaded = in.readLine();
            while( finishReadFlag){

                splitList = lineReaded.split(",",QString::SkipEmptyParts);
                timeSignal.append(splitList.at(0).toDouble());
                firstSignal.append(splitList.at(1).toDouble());
                secondSignal.append(splitList.at(2).toDouble());
                emit(dataToSignalProcessor(splitList.at(1).toInt(), splitList.at(2).toInt() ) );
                lineReaded = in.readLine();
                if(lineReaded.isNull()){
                    finishReadFlag = 0;
                }
                firstSignalCurve->setSamples(timeSignal,firstSignal);
                secondSignalCurve->setSamples(timeSignal,secondSignal);
                ui->widgetToPlot_2->replot();
            }


            file.close();

        }

    }else{
        qDebug() <<"Problema";
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if(index == 2){
        firstSignalCurve->detach();
        secondSignalCurve->detach();
        firstSignalCurve->attach(ui->widgetToPlot_2);
        secondSignalCurve->attach(ui->widgetToPlot_2);
    }else if (index == 1 ){
        firstSignalCurve->detach();
        secondSignalCurve->detach();
        firstSignalCurve->attach(ui->widgetToPlot);
        secondSignalCurve->attach(ui->widgetToPlot);
    }
}

void MainWindow::delay(int ms)
{
    QTime dieTime = QTime::currentTime().addMSecs( ms );
        while( QTime::currentTime() < dieTime )
        {
            QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
        }
}
