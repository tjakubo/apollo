#include "reciever.h"

Reciever::Reciever(Hardware *sourceHW, QWidget *parent) :
    QWidget(parent)
{
    qDebug() << connect(sourceHW, SIGNAL(NewMeasurement(meas)), this, SLOT(NewMeasurementRecieved(meas)));
    qDebug() << connect(sourceHW, SIGNAL(NewCalibrationData(meas,int)), this, SLOT(NewCalibrationDataRecieved(meas,int)));
    qDebug() << connect(sourceHW, SIGNAL(NewRawData(QString)), this, SLOT(NewRawDataRecieved(QString)));
}

void Reciever::NewCalibrationDataRecieved(meas calData, int sampleNum)
{
    Q_UNUSED(calData)
    Q_UNUSED(sampleNum)
}

void Reciever::NewRawDataRecieved(QString newRaw)
{
    Q_UNUSED(newRaw)
}
