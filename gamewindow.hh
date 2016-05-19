#ifndef GAMEWINDOW_HH
#define GAMEWINDOW_HH

#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QPainter>
#include <QElapsedTimer>
#include <QtCore/qmath.h>
#include <QTime>
#include <cstdlib>

#ifndef HARDWARE_H
#include "hardware.hh"
#define HARDWARE_H
#endif

#ifndef RECEIVER_H
#include "receiver.hh"
#define RECEIVER_H
#endif

class Ship
{
    QElapsedTimer Timer;

    double _legAngle;
    double _legLength;
    double _fragileRadius;

    double _xPos, _yPos, _ang;
    double _xVel, _yVel, _angVel;
    double _xAcc, _yAcc, _torq;

    double _maxThrust;
    double _currThrustPerc;

    double _maxTorq;
    double _currTorqPerc;

public:
    Ship();

    void Step();

    void Stop();
    void Steer(double newThrustPerc, double newTorqPerc);
    void AddForce(double xForce, double yForce, double torq);

    QPoint Pos();
    void SetPos(QPoint newPos);
    void SetPosRand(double maxCenterOffsetPerc, double minHeightPerc);

    double Angle();
    double AngleRad();
    //QVector<double> GetPos();

};

class Terrain
{
    QVector<QPoint> _verts;

public:
    Terrain(int xMin, int xMax, int vertCountMin, int vertCountMax, double dYmax);
    //void paintEvent(QPaintEvent *);
    const QVector<QPoint> Vec();
    int LowestElev();
    int ElevAtX(int xPos);
    double TiltAtX(int xPos);
};

namespace Ui {
class GameWindow;
}

class GameWindow : public Receiver
{
    Q_OBJECT
    Ship *_lander;
    Terrain *_terr;
    double _gravMag;

public:
    explicit GameWindow(Hardware *HWlink);
    ~GameWindow();
    void paintEvent(QPaintEvent *);
    void StepShip();

public slots:
    void NewMeasurementReceived(meas newMeas);

private slots:
    void on_pushButton_clicked();

private:
    Ui::GameWindow *ui;
};

#endif // GAMEWINDOW_HH
