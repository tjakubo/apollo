#include "receiver.hh"

Receiver::Receiver(Hardware *sourceHW, QWidget *parent) :
    QWidget(parent)
{
    bool connectOK = true;

    connectOK = connectOK && connect(sourceHW, SIGNAL(NewMeasurement(meas)), this, SLOT(NewMeasurementReceived(meas)));
    connectOK = connectOK && connect(sourceHW, SIGNAL(NewCalibrationData(meas,int)), this, SLOT(NewCalibrationDataReceived(meas,int)));
    connectOK = connectOK && connect(sourceHW, SIGNAL(NewRawData(QString)), this, SLOT(NewRawDataReceived(QString)));

    if(!connectOK)
        qDebug() << "Błąd podczas łączenia slotów odbioru danych ze sprzętu";
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
