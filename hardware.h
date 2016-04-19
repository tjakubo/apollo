#ifndef HARDWARE_H
#define HARDWARE_H

#include <QWidget>
#include <qstring.h>
#include <boost/asio.hpp>
#include <thread>
#include <qdebug.h>

struct meas
{
    int x, y, z;
};

class Hardware: public QWidget{
    Q_OBJECT
public:
    Hardware(QWidget *parent = nullptr);
    bool rawDataSent;

public slots:
    void Measure();
    void setRawDataStatus(bool isSent);

signals:
    void sendMeasurement(meas measurement);
    void sendRawData(QString data);

};

#endif // HARDWARE_H
