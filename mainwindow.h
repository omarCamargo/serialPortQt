#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <qserialport>
#include <QSerialPortInfo>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>

#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_plot_marker.h>
#include <qwt_series_data.h>
#include <qwt_plot_curve.h>

#include "serialportmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setupQWtPlotWidget();
    void clearQWtPlotWidget();
    void exportCurvesToFile();

private slots:
    void on_btnBuscar_clicked();

    void on_MylistWidget_doubleClicked(const QModelIndex &index);

    void on_btnConectar_clicked();

    void on_btnEnviar_clicked();

    void retrieveDataFromSerialPort();

    void processMesageFromSerial(QByteArray arr);

    void getErrorFromSerial(QSerialPort::SerialPortError err);



    void on_toolButton_clicked();

    void on_toolButton_3_clicked();

    void on_toolButton_2_clicked();

private:
    Ui::MainWindow *ui;
    SerialPortManager *manager;
    QString serialPortSelectedName;
    QString serialPortSelectedDescription;
    QSerialPort * Deviceserialport;
    bool conectedtoASerialPort;
    QList<QSerialPortInfo> serialList;

    QwtPlotCurve *firstSignalCurve;
    QwtPlotCurve *secondSignalCurve;
    QVector<double> firstSignal;
    QVector<double> secondSignal;
    QVector<double> timeSignal;
    int samplesCounter;

};

#endif // MAINWINDOW_H
