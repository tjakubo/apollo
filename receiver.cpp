#include "receiver.hh"

Receiver::Receiver(Hardware *sourceHW, QWidget *parent) :
    QWidget(parent)
{
    qDebug() << connect(sourceHW, SIGNAL(NewMeasurement(meas)), this, SLOT(NewMeasurementReceived(meas)));
    qDebug() << connect(sourceHW, SIGNAL(NewCalibrationData(meas,int)), this, SLOT(NewCalibrationDataReceived(meas,int)));
    qDebug() << connect(sourceHW, SIGNAL(NewRawData(QString)), this, SLOT(NewRawDataReceived(QString)));
}

void Receiver::NewCalibrationDataReceived(meas calData, int sampleNum)
{
    Q_UNUSED(calData)
    Q_UNUSED(sampleNum)
}

void Receiver::NewRawDataReceived(QString newRaw)
{
    Q_UNUSED(newRaw)
}
