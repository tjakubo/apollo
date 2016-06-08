#ifndef GAMEOBJECTS_HH
#define GAMEOBJECTS_HH

#define W_HEIGHT 650 ///< Stała - wysokość okna gry

#ifndef PHYSICS_HH
#include "physics.hh"
#define PHYSICS_HH
#endif

#include <vector>
#include <QPainter>
#include <QPoint>
#include <QElapsedTimer>
#include <QDebug>
#include <QPixmap>
#include <QRadialGradient>


/*! \brief Klasa terenu definiowanego jako krzywa łamana.
 *
 * Pozwala na zamknięcie całej obsługi interakcji z terenem w kilku Metodach:
 * Generuj losowy teren, generuj ponownie, wysokość nad współrzędną, nachylenie nad współrzędną.
 * Teren nie ma nic wspólnego z fizyką obiektów i domyślnie w ogóle nie wchodzi z nią w interakcje.
 */
class Terrain
{
    std::vector<QPoint> _verts; ///< Współrzędne wierzchołków terenu, układ z początkiem w lewym dolnym rogu!

public:
    //! Konstruktor parametryczny od razu wywołujacy generację terenu.
    Terrain(int xMin, int xMax, int vertCountMin, int vertCountMax, double dYmax);

    //! Metoda generujaca losowy teren z zadanymi parametrami.
    void Generate(int xMin, int xMax, int vertCountMin, int vertCountMax, double dYmax);

    //! Metoda zwracajaca stały wektor z wierzchołkami
    const std::vector<QPoint> Vec();

    //! Metoda zwracająca wysokość najniższego pktu terenu.
    int LowestElev();

    //! Metoda zwracająca wysokość terenu nad daną współrzędną x.
    int ElevAtX(int xPos);

    //! Metoda zwracająca nachylenie terenu nad argumentem.
    double TiltAtX(int xPos);

    //! Metoda rysująca teren podanym painterem.
    void Draw(QPainter *painter);

};

/*!
 * \brief Klasa cząsteczki rozchodzącej sie jak pył/dym i opcjonalnie kolidującej z terenem.
 *
 * Na cząsteczkę oddziałuje grawitacja, jej rozmiar i wygląd zależy od czasu życia oraz typu.
 * Nie posiada własnej funkcji destrukcji po czasie życia, jedynie może informować że czas ten minął.
 */
class Particle : public PhysicsObj
{
public:
    //! Możliwe typy cząsteczki (do rysowania)
    enum type { Flame, Spark, Dust };
private:
    type _partType;           ///< Typ cząsteczki (do rysowania)
    double _lifespanSec;      ///< pożądany czas życia w sekundach
    QElapsedTimer _lifeTimer; ///< timer odmierzający czas życia
    double _size;             ///< aktualna wielkość cząsteczki
    double _initSize;         ///< początkowa wielkość cząsteczki
    Terrain *_collTerrain;    ///< teren z którym cząsteczka wchodzi w interakcje, NULL jeśli ma nie wchodzić
public:
    //! Konstruktor parametryczny
    Particle(pos2d initPos, pos2d initVel, pos2d initGrav, type pType, double lifespanSec = 5, double size = 2);
    //! Wirtualny destruktor aby kompilator wiedział że tak ma być
    virtual ~Particle(){}

    //! Czy cząsteczka żyje dłużej niż zadany czas?
    bool Expired();

    //! Metoda rysująca cząsteczke podanym painterem (typ wg pola typu)
    void Draw(QPainter *painter);

    //! Metoda rysujaca czasteczke jako plamka ognia
    void DrawAsFlame(QPainter *painter);

    //! Metoda rysujaca czasteczke jako iskierke
    void DrawAsSpark(QPainter *painter);

    //! Metoda rysujaca czasteczke jako chmurkę pyłu
    void DrawAsDust(QPainter *painter);

    //! Metoda wykonująca "krok" cząsteczki (wielkość, kolizje)
    void Step(double dt = -1);

    //! Metoda ustalająca z którym obiektem terenu cząsteczka ma wchodzić w interakcje
    void SetTerrain(Terrain *terr);

};

/*!
 * \brief Klasa statku (lądownika) charakteryzująca jego parametry.
 *
 * Definiuje dane dotyczące usytuowania nóg lądownika, ich wytrzymałości, sił i momentów które
 * statek moze na siebie wywrzeć (sterowanie) itd.
 */
class Ship : public PhysicsObj
{
    double _legAngle;                 ///< nachylenie nóg lądownika od pionu (muszą być 2 symetryczne)
    double _legLength;                ///< długość nóg lądownika od jego środka
    double _legMaxImpactVel;          ///< prędkość po jakiej przekroczeniu nogi są niszczone przy kontakcie
    double _fragileRadius;            ///< maksymalna odległość kadłuba od terenu, mniejsza niszczy statek
    bool _leg1coll, _leg2coll;        ///< czy noga 1,2 są aktualnie w kontakcie z terenem
    int _state;                       ///< status:  1-zniszczony, 2-pierw. noga zniszczona, 3-druga noga zniszczona, reszta-OK
    double _maxFuelConsPerSec;        ///< pobór paliwa przy maksymalnej nastawie silnika
    double _maxFuelCap, _currFuel;    ///< maksymalny i aktualny poziom paliwa

    double _particleDensity;          ///< Współczynnik do skalowania ilosci tworzonych cząsteczek (0 - inf)

    double _maxThrust;                ///< maksymalna siła odrzutu silnika
    double _currThrustPerc;           ///< aktualny procent nastawy silnika

    double _maxTorq;                  ///< maskymalny moment obrotowy statku
    double _currTorqPerc;             ///< obecny procent nastawy obrotu statku

    std::vector<Particle*> _spawnedParticles; ///< wektor stworzonych przez statek cząsteczek

    Terrain *_terr;                   ///< teren z którym statek ma kolidować

public:
    //! Konstruktor parametryczny - TRZEBA podać teren dla kolizji dla statku
    Ship(Terrain *terr);
    //! Destruktor
    virtual ~Ship();

    //! Metoda wykonująca "krok" statku
    void Step(double dt = -1);

    //! Metoda ustawiająca nowe procentowe nastawy sterowania
    void Steer(double newThrustPerc, double newTorqPerc);

    //! Metoda rysująca statek oraz wywołujaca rysowanie jego cząsteczek
    void Draw(QPainter *painter);

    //! Metoda zwracająca nachylenie statku w stopniach w zakresie
    double Tilt(){ double tilt = Pos().AngDeg(); while(qAbs(tilt) > 180) tilt += 360*((tilt>0)?-1:1); return tilt; }

    //! Metoda ustalająca statkowi losową pozycję w granicach które są dobre dla rozpoczęcia gry
    void SetRandPos();

    //! Metoda tworząca chmurę cząsteczek o zadanej ilości, wielkości, prędkości, grawitacji i offsecie
    void SpawnPartCloud(int partNum, double partSize, double velMult = 1, double gravMult = 1,
                        Particle::type pType = Particle::type::Flame, QPoint offset = QPoint(0, 0));
    //! Metoda zadająca nowy współczynnik ilości tworzonych cząsteczek
    void SetPartDens(double newPartDens) { _particleDensity = newPartDens; }

    //! Metoda zwracająca wektor z informacjami o statku (dla okna do rysowania HUDu)
    std::vector<double>* ParseData();

    //! Metoda resetująca statek - stan OK, pełne paliwo, niszczy wszystkie cząsteczki
    void Reset();
};

//! Metoda transformujaca QPoint między układami współrzędnych.
QPoint Trans(QPoint p);

#endif // GAMEOBJECTS_HH

