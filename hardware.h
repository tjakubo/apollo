#ifndef HARDWARE_H
#define HARDWARE_H

#include <QWidget>
#include <qstring.h>
#include <boost/asio.hpp>
#include <thread>
#include <qdebug.h>

/*class Meas
{
    int x, y, z;
    int x_off, y_off, z_off;

public:
    Meas();
    const int x();
    const int y();
    const int z();
    void setMeas(int x, int y, int z);
    void setOffset(int x, int y, int z);
    void resetOffset();
    int xAngle();
    int yAngle();
};*/

struct meas
{
    int x, y, z;
    meas();
    meas(int nx, int ny, int nz);
};

meas operator+ (meas m1, meas m2);

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
