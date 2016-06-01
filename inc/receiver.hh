#ifndef RECEIVER_H
#define RECEIVER_H

#ifndef HARDWARE_H
#include "hardware.hh"
#define HARDWARE_H
#endif

#include <QWidget>

class Receiver : public QWidget
{
    Q_OBJECT
public:
    explicit Receiver(Hardware *sourceHW, QWidget *parent = 0);
    
public slots:
    virtual void NewMeasurementReceived(meas newMeas) = 0;
    virtual void NewCalibrationDataReceived(meas calData, int sampleNum);
    virtual void NewRawDataReceived(QString newRaw);
};

#endif // RECEIVER_H
