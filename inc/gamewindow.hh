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
#include <QRadialGradient>

#define W_HEIGHT 650

#ifndef HARDWARE_H
#include "hardware.hh"
#define HARDWARE_H
#endif

#ifndef RECEIVER_H
#include "receiver.hh"
#define RECEIVER_H
#endif

/// Funkcja transformujaca QPoint między układami współrzędnych.
/** Zamienia układ z początkiem w lewym dolnym rogu na układ z początkiem w lewym górnym rogu. */
QPoint Trans(QPoint p);

/* \brief Struktura zawierająca położenie i orientację punktu w przestrzeni 2D.
 * Stworzona w celu ujednolicenia sposobu przechowywania danych dot.
 * fizyki punktu materialnego, może być stosowana jako wektor z jedną
 * nadmiarową współrzędną.
 */
struct pos2d
{
    double x; ///< Wspołrzędna
    double y; ///< Współrzędna
    double ang; ///< Współrzędna, domyślnie w radianach

    pos2d() { x = y = ang = 0; } ///< Konstruktor bezparametryczny, zeruje współrzędne.
    pos2d(double nx, double ny, double na):x(nx),y(ny),ang(na){} ///< Konstruktor parametryczny

    //! Funkcja zwracająca pierwsze 2 współrzędne jako QPoint (dokładność integer)
    QPoint Point() const { return QPoint(x, y); }

    //! Funkcja zwracajaca długość wektora (x,y), ignoruje 3. wpsółrzędną
    double Mag() const { return sqrt(pow(x, 2) + pow(y, 2)); }

    //! Funkcja zwracająca kąt (3. współrzędna) w radianiach
    double AngRad() const { return ang; }
    //! Funkcja zwracająca kąt (3. współrzędna) w stopniach
    double AngDeg() const { return qRadiansToDegrees(ang); }

    //! Dodawanie struktur współrzędna po współrzędnej
    pos2d operator+(const pos2d& p) { return pos2d(x+p.x, y+p.y, ang+p.ang); }
    //! Mnożenie struktur współrzędna po współrzędnej
    pos2d operator*(double m) { return pos2d(x*m, y*m, ang*m); }
    //! Mnożenie struktur współrzędna po współrzędnej
    pos2d operator*(double m) const { return pos2d(x*m, y*m, ang*m); }
};

//! Dodawanie struktur współrzędna po współrzędnej
pos2d operator+(const pos2d& p1, const pos2d& p2);

/*! \brief Klasa implementujaca fizykę punktu materialnego w 2D (2 współrzędne + kąt obrotu)
 * Operuje na strukturach zawierających potrzebne współrzędne.
 * Fizyka opiera się na prostym sumowaniu prędkości do położenia i siły do prędkości.
 * NIE posiada właściwości masy (działa jakby masa=1), należy ją uwzględnić przed podaniem siły sterujacej.
 * Funkcja wykonująca następny "krok" fizyki NIE jest wywoływana automatycznie - wbudowany timer służy tylko
 * do mierzenia odcinka czasu między wywołaniami.
 * Nie jest czysto wirtualna, ale stworzona głównie po to aby inne klasy mogły łatwo dziedziczyć
 * fizykę punktu materialnego.
 * Nie obsługuje kolizji.
 */
class PhysicsObj
{
    pos2d _pos; ///< Pozycja i orientacja punktu
    pos2d _vel; ///< Prędkość liniowa i kątowa punktu
    pos2d _acc; ///< Siły i moment działajace na punkt
    pos2d _gravity; /// < Siła grawitacji
    QElapsedTimer _timer; ///< Timer mierzący odcinki czasu pomiędzy krokami
    bool _firstRun; ///< Pomocnicza zmienna do inicjalizacji timera

public:
    //! asdasd
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
    double _initSize;
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
    double _maxFuelConsPerSec;
    double _maxFuelCap, _currFuel;

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
    void SpawnPartCloud(int partNum, double partSize, double velMult = 1, double gravMult = 1, QPoint offset = QPoint(0, 0));

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
