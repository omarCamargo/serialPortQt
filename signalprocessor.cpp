#include "signalprocessor.h"

SignalProcessor::SignalProcessor(QObject *parent) : QObject(parent)
{
    resetVals();

}

void SignalProcessor::processData(int &sdc,int &sac)
{
    if(stopSearchCalc ){
        stopSearchCalc=0;
        correctExtremes();
        correctMinors();
        correctRepeted();
        calcDiast();
        calcSist();
        calcMed();
        emit(diastolicAndSistolicWasFound(diast,sist));
        return;
    }
    if(sdc >140){//Valor ADC Mayor a los valores
        if(maxv<sac){
            maxv=sac;                                                        //hace maximo igual al valor actual
            tmax=dataCounter;                                                       //almacena el tiempo de ese maximo
            dc=sdc;                                                           //almacena el valor dc del maximo
            cont_x=0;
        }else{//si el valor no es un maximo
            cont_x=cont_x+1;                                                    //aumenta el contador de valores no maximos
        }
        
        if(cont_x>20) {                                                          //si han pasado mas de 12 datos que no son maximos
            if(band==1){                                                       //si lo ultimo determinado fue un maximo
                if(v_max<maxv){                                                  //si el valor almacenado anteriormente es menor que el actual
                    v_max=maxv;                                                 //guarda mejor este ultimo valor
                    v_dc=dc;                                                     //guarda el otro nivel dc
                }
            }else {                                                               //si lo ultimo determinado fue un minimo
                    v_max=maxv;                                                     //guarda el nuevo valor en el vector de maximos
                    v_tmax=tmax;                                                    //guarda el tiempo de ese maximo
                    v_dc=dc;                                                        //guarda el otro nivel dc
                    //plot(tmax,maxv);   //emitir senal de ploteo                                       //grafica el punto como maximo
                }
                maxv=-1800;  
                cont_x=0;                                                           //resetea el contador de no maximos
                band=1;  



        }//fin de if que cuenta 12 valores que no son maximos
        
        //deteccion de minimos
        if(minv>sac ){                                                        //si el valor maximo es menor al valor de presion
            minv=sac;                                                        //hace maximo igual al valor actual
            tmin=dataCounter;                                                       //almacena el tiempo de ese maximo
            cont_m=0;                                                           //reinicia el contador de valores no minimos
        }else{                                                                    //si el valor no es un minimo
            cont_m=cont_m+1;                                                    //aumenta el contador de valores no minimos
        }
        
        if(cont_m > 20){
            
            if(band==0){                                                         //si lo ultimo que ingreso fue un minimo
                if(v_min>minv){                                                  //si el valor almacenado anteriormente es mayor que el actual
                    v_min=minv;                                                 //almacena mejor el nuevo valor como minimo
                }
            }else{
                if(cont_min>0){                                                  //si ha medido mas de 1 minimo, es porque debe haber por lo menos un maximo y puede calcular una amplitud
                    if(v_max<minv){
                        v_max=v_max;
                    }
                    
                    if(v_max-v_min>40 && v_max-minv>80){
                        amp.append(v_max-v_min);                              //almacena la amplitud
                        maxs.append(v_max);                                      //guarda el valor maximo
                        mins.append(v_min);                                      //guarda el valor minimo
                        tm.append(v_tmin);
                        tx.append(v_tmax);
                        pressures.append(v_dc);
                        cont_amp=cont_amp+1;                                    //aumeta el contador de amplitudes almacenadas
                        borrar_min=0;
                        calcMaxOfAmps();
                    }
                    cont_min=0;                                                 //reinicia el contador de minimos
                }//Fin de cont_min >0
                
                if(borrar_min==0){
                    v_min=minv;                                                 //guarda el nuevo valor como minimo
                    v_tmin=tmin;                                                //guarda el tiempo de ese minimo
                    cont_min=cont_min+1;                                        //aumenta el contador de posiciones del vector de minimos
                    //hold on
                    //plot(tmin, minv);
                    borrar_min=1;
                }else{
                    cont_min=1;                                                 //indica que hay un minimo
                }
                
            }//Fin de else que calcula amplitudes
            minv=3600; 
            cont_m=0;                                                           //reinicia el contador de no minimos
            band=0;
            
        }//FIN DE contador_m > 12
        
    }//FIN de if que chequea si sdc mayor a 140

}

void SignalProcessor::resetVals()
{
    stopSearchCalc=0;
    minv = 1023;                                                                      //minimo valor de voltaje por defecto
    maxv = 0;                                                                         //maximo valor de voltaje por defecto
    cont_min = 0;                                                                     //contador de minimos
    band = 2;                                                                         //bandera que identifica si la ultima medida fue maximo o minimo, por defecto ninguna
    cont_x = 0;                                                                       //contador de valores no maximos
    cont_m = 0;                                                                       //contador de valores no minimos
    v_min = 0;                                                                        //valor de minimos
    v_max = 0;                                                                        //valor de maximos
    cont_amp = 1;
    cont_dc = 0;
    borrar_min = 0;
    dataCounter = 0;
    amp.clear();
    pressures.clear();
    mins.clear();
    maxs.clear();
    tm.clear();
    tx.clear();
}

void SignalProcessor::calcMaxOfAmps()
{
    int max =0;
    if(amp.length() > 2){
        max = *std::max_element( amp.constBegin(),amp.constEnd());
        indexOfMaxAmp = amp.indexOf(max);
        minAmpBeforeStop = (max*45)/100;
        if(minAmpBeforeStop > amp.last() ){
            stopSearchCalc =1;
            cont_amp--;
        }
    }
}

void SignalProcessor::processNewValueAcquired(int sdc, int sac)
{
    dataCounter++;
    processData(sdc,sac);
}
int SignalProcessor::getOffset() const
{
    return offset;
}

void SignalProcessor::setOffset(int value)
{
    offset = value;
}


void SignalProcessor::correctExtremes()
{
    int first = amp.at(0);
    int second = amp.at(1);
    int third = amp.at(2);

    int last = amp.last();
    int lastMinusOne = amp.at(amp.length()-2);
    int lastMinusTwo = amp.at(amp.length()-3);
    //Primeros Valores
    if(first >= second && first >= third ){
        if( second < third ){
            amp[0] = second;
        }else{
            amp[0] = third;
        }

    }

    //Ultimos valores
    if( ( last >= lastMinusOne ) && ( last >= lastMinusTwo )){
        if( lastMinusOne < lastMinusTwo ){
            amp[amp.length() -1 ] = lastMinusOne;
        }else{
            amp[amp.length() -1 ] = lastMinusTwo;
        }
    }
}

void SignalProcessor::correctRepeted()
{
    for (int i=1; i < amp.length()-1;i++ ){
        if(amp.at(i) == amp.at(i-1)){
            amp[i] =  (amp.at(i+1) + amp.at(i-1))/2;
        }
    }

}

void SignalProcessor::correctMinors()
{
    for(int i=0; i < amp.length()/2;i++ ){
        for(int j = indexOfMaxAmp + 1; j < amp.length()-1;j++){
            if(amp.at(j) < amp.at(j+1) &&  amp.at(j) < amp.at(j-1) ){
                amp[j] =  (amp.at(j+1) + amp.at(j-1))/2;
            }
        }

        for(int k = indexOfMaxAmp-1; k > 1;k--){
            if(amp.at(k) < amp.at(k+1) &&  amp.at(k) < amp.at(k-1) ){
                amp[k] =  (amp.at(k+1) + amp.at(k-1))/2;
            }
        }


    }
}

void SignalProcessor::calcSist()
{
    int band_sist =0;
    int val_adc_Psist;
    int vs = (5*amp.at(indexOfMaxAmp))/10;
    int i;
    for(i = indexOfMaxAmp; i < amp.length()-2;i++ ){
        if(vs <= amp.at(i) && vs >= amp.at(i+1)  ){
            int n = (amp.at(i)-amp.at(i+1))/(pressures.at(i+1)-pressures.at(i));
            val_adc_Psist = (((amp.at(i)-vs))/n)+pressures.at(i);
            band_sist=1;
        }
    }
    if(band_sist == 0){
        val_adc_Psist =pressures.at(i+1);
    }
    sist = ((val_adc_Psist - getOffset())*1250)/3069;

}

void SignalProcessor::calcDiast()
{
    int band_diast = 0;
    int band_diast2 = 0;
    int val_adc_Pdiast;
    int val_adc_Pdiast2;
    int vd1 = (69*amp.at(indexOfMaxAmp))/100;
    int vd2 = (55*amp.at(indexOfMaxAmp))/100;
    int pdiast;

    for(int i = 1; i < indexOfMaxAmp;i++ ){
        if(vd1 >= amp.at(i-1) && vd1 <= amp.at(i)  ){
            int n = (amp.at(i)-amp.at(i-1))/(pressures.at(i)-pressures.at(i-1));
            val_adc_Pdiast = pressures.at(i)-( (amp.at(i)-vd1)/n );
            band_diast=1;
        }

    }
    if (band_diast == 0 ){
        val_adc_Pdiast = pressures.at(cont_amp);
    }

    for(int i = 1; i < indexOfMaxAmp;i++ ){
        if(vd1 >= amp.at(i-1) && vd1 <= amp.at(i)  ){
            int n = (amp.at(i)-amp.at(i-1))/(pressures.at(i)-pressures.at(i-1));
            val_adc_Pdiast2 = pressures.at(i)-( (amp.at(i)-vd2)/n );
            band_diast2=1;
        }

    }
    if (band_diast2 == 0 ){
        val_adc_Pdiast2 = pressures.at(cont_amp);
    }

    pdiast=(val_adc_Pdiast+val_adc_Pdiast2)/2;

    diast = (pdiast-getOffset())*1250/3069;
}

void SignalProcessor::calcMed()
{
    int band_med = 0;
    int p;
    int pmedia;
    for(int i = 0; i < indexOfMaxAmp ;i++){
        if(amp.at(indexOfMaxAmp) >= amp.at(i) &&  amp.at(indexOfMaxAmp+1)<=amp.at(i+1)){
            p=(amp.at(i+1)-amp.at(i))/(pressures.at(i+1)-pressures.at(i));
            pmedia = pressures.at(i)+((amp.at(indexOfMaxAmp+1)-amp.at(i))/(p+1));
            band_med=1;
        }
    }
    if(band_med==0){
        p=(amp.at(1)-amp.at(0))/(pressures.at(1)-pressures.at(0));
        pmedia=pressures.at(0)-((amp.at(0)-amp.at(indexOfMaxAmp+1))*p);
    }

    median = ((pmedia - offset)*1250)/3069;

}



