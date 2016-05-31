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
#include <QImage>
#include <QPixmap>
#include <QPolygon>
#include <QPainterPath>

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
    double Mag() const { return sqrt(pow(x, 2) + pow(y, 2)); }
    double AngRad() const { return ang; }
    double AngDeg() const { return qRadiansToDegrees(ang); }
    pos2d operator+(const pos2d& p) { return pos2d(x+p.x, y+p.y, ang+p.ang); }
    pos2d operator*(double m) { return pos2d(x*m, y*m, ang*m); }
    pos2d operator*(double m) const { return pos2d(x*m, y*m, ang*m); }
};

pos2d operator+(const pos2d& p1, const pos2d& p2);

class PhysicsObj
{
    pos2d _pos;
    pos2d _vel;
    pos2d _acc;
    pos2d _gravity;
    QElapsedTimer _timer;
    bool _firstRun;

public:

    PhysicsObj();
    PhysicsObj(pos2d initPos, pos2d initVel, pos2d initGrav);

    virtual void Step(double dt = -1);
    const pos2d& Pos(){ return _pos; }
    void SetPos(pos2d newPos){ _pos = newPos; }
    const pos2d& Vel() const { return _vel; }
    const pos2d& Grav() const {return _gravity; }

    void Stop();
    pos2d Acc(){ return _acc; }
    void SetAcc(pos2d newAcc){ _acc = newAcc; }
    void SetVel(pos2d newVel) { _vel = newVel; }
    void AddForce(pos2d aForce);
    void RotateVel(double angDeg);

};


class Terrain
{
    QVector<QPoint> _verts;
    QVector<double> _slopes;

public:
    Terrain(int xMin, int xMax, int vertCountMin, int vertCountMax, double dYmax);
    //void paintEvent(QPaintEvent *);
    const QVector<QPoint> Vec();
    int LowestElev();
    int ElevAtX(int xPos);
    double TiltAtX(int xPos);

    void Draw(QPainter *painter);

};

class Particle : public PhysicsObj
{
    double _lifespanSec;
    QElapsedTimer _lifeTimer;
    double _size;
    Terrain *_collTerrain;
public:
    Particle(pos2d initPos, pos2d initVel, pos2d initGrav, double lifespanSec = 5, double size = 2);
    bool Expired();
    void Draw(QPainter *painter);
    void Step(double dt = -1);
    void SetTerrain(Terrain *terr);

};

class Ship : public PhysicsObj
{
    double _legAngle;
    double _legLength;
    double _legMaxImpactVel;
    double _fragileRadius;
    bool _leg1coll, _leg2coll;
    int _state;

    double _maxThrust;
    double _currThrustPerc;

    double _maxTorq;
    double _currTorqPerc;

    std::vector<Particle*> _spawnedParticles;

    Terrain *_terr;

public:
    Ship(Terrain *terr);

    void Step(double dt = -1);

    //void Stop();
    void Steer(double newThrustPerc, double newTorqPerc);

    void Draw(QPainter *painter);

    //QPoint Pos();
    //void SetPos(QPoint newPos);
    void SetRandPos();
    void SpawnPartCloud(int partNum, double velMult = 1, double gravMult = 1, QPoint offset = QPoint(0, 0));

    std::vector<double>* ParseData();
    void Reset();

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
    void StepScene();

public slots:
    void NewMeasurementReceived(meas newMeas);

private slots:
    void on_pushButton_clicked();

private:
    Ui::GameWindow *ui;
};

#endif // GAMEWINDOW_HH
