#ifndef RECIEVER_H
#define RECIEVER_H

#ifndef HARDWARE_H
#include "hardware.hh"
#define HARDWARE_H
#endif

#include <QWidget>

class Reciever : public QWidget
{
    Q_OBJECT
public:
    explicit Reciever(Hardware *sourceHW, QWidget *parent = 0);
    
public slots:
    virtual void NewMeasurementRecieved(meas newMeas) = 0;
    virtual void NewCalibrationDataRecieved(meas calData, int sampleNum);
    virtual void NewRawDataRecieved(QString newRaw);
};

#endif // RECIEVER_H
