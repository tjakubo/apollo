#ifndef PHYSICS_HH
#define PHYSICS_HH

#include <QElapsedTimer>
#include <QPoint>
#include <QtCore/qmath.h>

/*! \brief Struktura zawierająca położenie i orientację punktu w przestrzeni 2D.
 *
 * Stworzona w celu ujednolicenia sposobu przechowywania danych dot.
 * fizyki punktu materialnego, może być stosowana jako wektor z jedną
 * nadmiarową współrzędną.
 */
struct pos2d
{
    double x; ///< Wspołrzędna
    double y; ///< Współrzędna
    double ang; ///< Współrzędna, domyślnie w radianach

    pos2d() { x = y = ang = 0; } ///< Konstruktor bezparametryczny, zeruje współrzędne
    pos2d(double nx, double ny, double na):x(nx),y(ny),ang(na){} ///< Konstruktor parametryczny

    //! Metoda zwracająca pierwsze 2 współrzędne jako QPoint (dokładność integer)
    QPoint Point() const { return QPoint(x, y); }

    //! Metoda zwracajaca długość wektora (x,y), ignoruje 3. współrzędną
    double Mag() const { return sqrt(pow(x, 2) + pow(y, 2)); }

    //! Metoda zwracająca kąt (3. współrzędna) w radianiach
    double AngRad() const { return ang; }
    //! Metoda zwracająca kąt (3. współrzędna) w stopniach
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

/*! \brief Klasa implementujaca fizykę punktu materialnego w 2D (2 współrzędne + kąt obrotu).
 *
 * Operuje na strukturach zawierających potrzebne współrzędne. <br>
 * Fizyka opiera się na prostym sumowaniu prędkości do położenia i siły do prędkości. <br>
 * NIE posiada właściwości masy (działa jakby masa=1), należy ją uwzględnić przed podaniem siły sterujacej. <br>
 * Metoda wykonująca następny "krok" fizyki NIE jest wywoływana automatycznie - wbudowany timer służy tylko
 * do mierzenia odcinka czasu między wywołaniami. <br>
 * Nie jest czysto wirtualna, ale stworzona głównie po to aby inne klasy mogły łatwo dziedziczyć
 * fizykę punktu materialnego. <br>
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
    //! Konstruktor bezparametryczny
    PhysicsObj();
    //! Konstruktor parametryczny
    PhysicsObj(pos2d initPos, pos2d initVel, pos2d initGrav);

    //! Metoda wykonująca "krok" fizyki obiektu
    virtual void Step(double dt = -1);

    //! Pobranie referencji do stałego obiektu pozycji
    const pos2d& Pos(){ return _pos; }
    //! Metoda zadająca nową pozycję
    void SetPos(pos2d newPos){ _pos = newPos; }
    //! Pobranie referencji do stałego obiektu predkosci
    const pos2d& Vel() const { return _vel; }
    //! Pobranie referencji do stałego obiektu przyspieszenia
    const pos2d& Acc() const { return _acc; }
    void SetAcc(pos2d newAcc){ _acc = newAcc; }
    //! Pobranie referencji do stałego obiektu grawitacji
    const pos2d& Grav() const {return _gravity; }
    //! Metoda ustawiająca nową wartość grawitacji
    void SetGrav(pos2d newGrav) { _gravity = newGrav; }

    //! Metoda zerujaca prędkość obiektu
    void Stop();

    //! Metoda dodająca do aktualnego przyspieszenia nowe przyspieszenie
    void AddForce(pos2d aForce);

    //! Metoda obracająca wektor prędkości o kąt w stopniach.
    void RotateVel(double angDeg);

};

#endif // PHYSICS_HH

