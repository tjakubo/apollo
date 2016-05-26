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

#define W_HEIGHT 650

#ifndef HARDWARE_H
#include "hardware.hh"
#define HARDWARE_H
#endif

#ifndef RECEIVER_H
#include "receiver.hh"
#define RECEIVER_H
#endif

QPoint Trans(QPoint p);

struct pos2d
{
    double x, y, ang;

    pos2d() { x = y = ang = 0; }
    pos2d(double nx, double ny, double na):x(nx),y(ny),ang(na){}
    QPoint Point() const { return QPoint(x, y); }
    double Mag(){ return sqrt(pow(x, 2) + pow(y, 2)); }
    double AngRad(){ return ang; }
    double AngDeg(){ return qRadiansToDegrees(ang); }
    pos2d operator+(const pos2d& p){ return pos2d(x+p.x, y+p.y, ang+p.ang); }
    pos2d operator*(double m) { return pos2d(x*m, y*m, ang*m); }
};

pos2d& operator+(const pos2d& p1, const pos2d& p2);

class PhysicsObj
{
    pos2d _pos;
    pos2d _vel;
    pos2d _acc;
    pos2d _gravity;
    QElapsedTimer _timer;

public:

    PhysicsObj();
    PhysicsObj(pos2d initPos, pos2d initGrav);

    virtual void Step(double dt = -1);
    const pos2d& Pos(){ return _pos; }
    const pos2d& Vel(){ return _vel; }

    pos2d& Acc(){ return _acc; }
    void AddForce(pos2d aForce);

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

    void Draw(QPainter *painter);

};

class Ship : public PhysicsObj
{
    double _legAngle;
    double _legLength;
    double _fragileRadius;

    double _maxThrust;
    double _currThrustPerc;

    double _maxTorq;
    double _currTorqPerc;

    Terrain *_terr;

public:
    Ship(Terrain *terr);

    void Step(double dt = -1);

    //void Stop();
    void Steer(double newThrustPerc, double newTorqPerc);

    void Draw(QPainter *painter);

    void Stop();


    //QPoint Pos();
    //void SetPos(QPoint newPos);
    //void SetPosRand(double maxCenterOffsetPerc, double minHeightPerc);

    //double Angle();
    //double AngleRad();
    //QVector<double> GetPos();

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
