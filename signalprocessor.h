#ifndef SIGNALPROCESSOR_H
#define SIGNALPROCESSOR_H

#include <QObject>
#include <QVector>


class SignalProcessor : public QObject
{
    Q_OBJECT
public:
    explicit SignalProcessor(QObject *parent = 0);
    void processData(int &sdc, int &sac);
    void resetVals();
    void calcMaxOfAmps();

    int getOffset() const;
    void setOffset(int value);

signals:
    void diastolicAndSistolicWasFound(int pd, int ps);

public slots:
    void processNewValueAcquired(int sdc, int sac);

private:
    QVector<int> amp;
    QVector<int> pressures;
    QVector<int> mins;
    QVector<int> tm;
    QVector<int> maxs;
    QVector<int> tx;


    int minv;
    int maxv;
    int cont_min;
    int band;
    int cont_x;
    int cont_m;
    int v_min;
    int v_max;
    
    int cont_amp;
    int cont_dc;
    int borrar_min;
    int dataCounter;
    int sist;
    int diast;
    int median;
    int offset;

    //VAriables Usadas para calcular caules valores pueden ser candidatos a ser maximos o minimos// OJO SUPERPRIVADAS NO TOCAR
    int tmax;
    int tmin;//almacena el tiempo de ese maximo
    int dc;
    int v_dc;
    int v_tmax;
    int v_tmin;
    unsigned char stopSearchCalc;
    int indexOfMaxAmp;
    int minAmpBeforeStop;

    void correctExtremes();
    void correctRepeted();
    void correctMinors();
    void calcSist();
    void calcDiast();
    void calcMed();

};

#endif // SIGNALPROCESSOR_H
