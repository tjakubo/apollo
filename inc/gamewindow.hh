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

/// Metoda transformujaca QPoint między układami współrzędnych.
/** Zamienia układ z początkiem w lewym dolnym rogu na układ z początkiem w lewym górnym rogu. Korzysta ze stałej W_HEIGHT.*/
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

    //! Metoda zwracająca pierwsze 2 współrzędne jako QPoint (dokładność integer)
    QPoint Point() const { return QPoint(x, y); }

    //! Metoda zwracajaca długość wektora (x,y), ignoruje 3. wpsółrzędną
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
 * Operuje na strukturach zawierających potrzebne współrzędne.
 * Fizyka opiera się na prostym sumowaniu prędkości do położenia i siły do prędkości.
 * NIE posiada właściwości masy (działa jakby masa=1), należy ją uwzględnić przed podaniem siły sterujacej.
 * Metoda wykonująca następny "krok" fizyki NIE jest wywoływana automatycznie - wbudowany timer służy tylko
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
    //! Konstruktor bezparametryczny
    PhysicsObj();
    //! Konstruktor parametryczny
    PhysicsObj(pos2d initPos, pos2d initVel, pos2d initGrav);

    /*! \brief Metoda wykonująca "krok" fizyki obiektu.
     *
     * Działa jak prosta aproksymacja y(t+dt)=y(t)+(dy/dt)*dt, dt jest brane z argumentu lub timera,
     * (dy/dt) to Vel dla Pos i Acc dla Vel. Siła (Acc) pozostaje niezmieniona.
     */
    virtual void Step(double dt = -1);

    //! Pobranie referencji do stalego obiektu pozycji
    const pos2d& Pos(){ return _pos; }
    //! Funnkcja zadająca nową pozycję
    void SetPos(pos2d newPos){ _pos = newPos; }
    //! Pobranie referencji do stalego obiektu predkosci
    const pos2d& Vel() const { return _vel; }
    //! Pobranie referencji do stalego obiektu przyspieszenia
    const pos2d& Acc() const { return _acc; }
    void SetAcc(pos2d newAcc){ _acc = newAcc; }
    //! Pobranie referencji do stalego obiektu grawitacji
    const pos2d& Grav() const {return _gravity; }

    /*! \brief Metoda zerujaca prędkość obiektu.
     *
     * Oprócz wyzerowania prędkości, restartuje ona także wewnętrzny timer aby zapobiec "nawijaniu"
     * sie miniętego czasu kiedy chcemy w programie np. wybierać warunkowo między wywołaniem Step() a Stop().
     */
    void Stop();

    //! Metoda dodająca do aktualnego przyspieszenia nowe przyspieszenie
    void AddForce(pos2d aForce);

    /*! \brief Metoda obracająca wektor prędkości o kąt w stopniach.
     *
     * Współrzędne x,y pola _vel są traktowane jako zwykły wektor i obracane wokół początku układu
     * współrzędnych.
     * Pole kąta (_ang) nie ulega zmianie.
     */
    void RotateVel(double angDeg);

};

/*! \brief Klasa terenu definiowanego jako krzywa łamana.
 *
 * Pozwala na zamknięcie całej obsługi interakcji z terenem w kilku Metodach:
 * Generuj losowy teren, generuj ponownie, wysokość nad współrzędną, nachylenie nad współrzędną.
 * Teren nie ma nic wspólnego z fizyką obiektów i domyślnie w ogóle nie wchodzi z nią w interakcje.
 */
class Terrain
{
    QVector<QPoint> _verts; ///< Współrzędne wierzchołków terenu, układ z początkiem w lewym dolnym rogu!

public:
    /*! \brief Konstruktor parametryczny, generuje losowy tern z zadanymi parametrami.
     *
     * \param xMin Od jakiej współrzędnej x ma się rozpoczynać teren
     * \param xMax Do jakiej współrzędnej x ma być teren
     * \param verCountMin Minimalna liczba segmentów terenu
     * \param vertCountMax Maksymalna liczba segmentów terenu
     * \param dYmax Moduł maksymalnego nachylenia terenu (pochodnej prostej)
     */
    Terrain(int xMin, int xMax, int vertCountMin, int vertCountMax, double dYmax);

    //! Metoda zwracajaca stały wektor z wierzchołkami
    const QVector<QPoint> Vec();

    //! Metoda zwracająca wysokość najniższego pktu terenu.
    int LowestElev();
    /*! \brief Metoda zwracająca wysokość terenu nad daną współrzędną x.
     *
     * Jeśli poda się zbyt duży lub mały argument, zwraca wysokość nad najbliższym poprawnym punktem
     * (nie ekstrapoluje ostatniego odcinka)
     */
    int ElevAtX(int xPos);
    /*!
     * \brief Metoda zwracająca nachylenie terenu nad argumentem.
     *
     * Korzysta z funkcji atan2(dy,dx)
     * \return Nachylenie terenu w RADIANACH
     */
    double TiltAtX(int xPos);

    /*!
     * \brief Metoda rysująca teren podanym painterem.
     *
     * Teren rysowany jest w postaci wypełnionego wieloboku z wierzchołkami w wierzchołkach terenu
     * oraz jednym w prawym dolnym rogu okna, drugim w lewym dolnym rogu.
     */
    void Draw(QPainter *painter);

};

/*!
 * \brief Klasa cząsteczki rozchodzącej sie jak pył/dym i opcjonalnie kolidującej z terenem.
 *
 * Na cząsteczkę oddziałuje grawitacja, rozszerza się ona wraz z czasem życie jednocześnie stając się
 * coraz bardziej przezroczysta.
 * Nie posiada własnej funkcji destrukcji po czasie życia, jedynie może informować że czas ten minął.
 */
class Particle : public PhysicsObj
{
    double _lifespanSec;      ///< pożądany czas życia w sekundach
    QElapsedTimer _lifeTimer; ///< timer odmierzający czas życia
    double _size;             ///< aktualna wielkość cząsteczki
    double _initSize;         ///< początkowa wielkość cząsteczki
    Terrain *_collTerrain;    ///< teren z którym cząsteczka wchodzi w interakcje, NULL jeśli ma nie wchodzić
public:

    //! Konstruktor parametryczny
    Particle(pos2d initPos, pos2d initVel, pos2d initGrav, double lifespanSec = 5, double size = 2);
    //! Wirtualny destruktor aby kompilator wiedział że tak ma być
    virtual ~Particle(){}

    //! Czy cząsteczka żyje dłużej niż zadany czas?
    bool Expired();

    /*!
     * \brief Metoda rysująca cząsteczke podanym painterem
     *
     * Automatycznie dostosowane są: wielkość cząsteczki (rośni z czasem), kolor cząsteczki
     * (czerwony->żółty z czasem), przezroczystość cząsteczki (gradientowa, w centrum 0-100% z czasem)
     */
    void Draw(QPainter *painter);

    /*!
     * \brief Metoda wykonująca "krok" cząsteczki (wielkość, kolizje)
     *
     * Metoda ta obsługuje zmianę wielkości cząsteczki od czasu życia oraz kolizje z
     * terenem, o ile ten został ustawiony.
     * Kolizja z terenem polega na zmianie prędkości na równoległą do terenu (bez strat w module).
     * Kolizje pod kątem ostrzejszym nic 10 stopni są ignorowane.
     * Potem wywoływany jest krok fizyki cząsteczki z zadanym odcinkiem czasu.
     *
     * \param dt Zadana długość kroku do obliczeń dla fizyki cząsteczki (tylko dla klasy bazowej)
     */
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

    double _maxThrust;                ///< maksymalna siła odrzutu silnika
    double _currThrustPerc;           ///< aktualny procent nastawy silnika

    double _maxTorq;                  ///< maskymalny moment obrotowy statku
    double _currTorqPerc;             ///< obecny procent nastawy obrotu statku

    std::vector<Particle*> _spawnedParticles; ///< wektor stworzonych przez statek cząsteczek

    Terrain *_terr;                   ///< teren z którym statek ma kolidować

public:
    //! Konstruktor parametryczny - TRZEBA podać teren dla kolizji dla statku
    Ship(Terrain *terr);

    /*!
     * \brief Metoda wykonująca "krok" statku
     *
     * Funkcja ta obsługuje większą część całej funkcjonalnosci programu:
     * - obsługuje kolizje statku z terenem, poprzez nogi i bez nich
     * - obsługuje imitację "spadania" lądownika na drugą nogę gdy stio na jednej
     * - obsługuje utratę paliwa w czasie
     * - tworzy cząsteczki imitujące gazy odrzutowe, wybuchy, ogień po zniszczeniu
     * - wywołuje fizykę cząsteczek
     *
     * \param dt Zadana długość kroku do obliczeń dla fizyki statku i cząsteczek
     */
    void Step(double dt = -1);

    //void Stop();
    //! Metoda ustawiająca nowe procentowe nastawy sterowania
    void Steer(double newThrustPerc, double newTorqPerc);

    /*!
     * \brief Metoda rysująca statek oraz wywołujaca rysowanie jego cząsteczek
     *
     * Oprócz rysowania cząsteczek, obsługuje również niszczenie tych, którym minął czas życia
     */
    void Draw(QPainter *painter);

    //! Metoda zwracająca nachylenie statku w stopniach w zakresie
    double Tilt(){ double tilt = Pos().AngDeg(); while(qAbs(tilt) > 180) tilt += 180*((tilt>0)?-1:1); return tilt; }

    //! Metoda ustalająca statkowi losową pozycję w granicach które są dobre dla rozpoczęcia gry
    void SetRandPos();

    //! Metoda tworząca chmurę cząsteczek o zadanej ilości, wielkości, prędkości, grawitacji i offsecie
    void SpawnPartCloud(int partNum, double partSize, double velMult = 1, double gravMult = 1, QPoint offset = QPoint(0, 0));

    //! Metoda zwracająca wektor z informacjami o statku (dla okna do rysowania HUDu)
    std::vector<double>* ParseData();

    //! Metoda resetująca statek - stan OK, pełne paliwo, niszczy wszystkie cząsteczki
    void Reset();

    //double Angle();
    //double AngleRad();
    //QVector<double> GetPos();

};

namespace Ui {
class GameWindow;
}

/*!
 * \brief Klasa zarządzająca oknem gry
 *
 * Tworzy obiekty lądownika i terenu oraz zarządza przesyłem pomiarów (sterowania) od sprzętu
 * do lądownika. Definiuje wielkość grawitacji dla lądownika
 */
class GameWindow : public Receiver
{
    Q_OBJECT
    Ship *_lander;     ///< Wskaźnik na lądownik na scenie
    Terrain *_terr;    ///< Wskaźnik na teren na scenie
    double _gravMag;   ///< Wielkość grawitacji (ustalony kierunek w  dół)

public:
    /*!
     * \brief Konstruktor parametryczny
     * \param HWlink Wskaźnik na obiekt klasy sprzętu d którego ma brać pomiary dla sterowania
     */
    explicit GameWindow(Hardware *HWlink);
    //! Destruktor
    ~GameWindow();

    /*!
     * \brief Metoda rysująca HUD oraz wywołująca rysowanie terenu i lądownika
     *
     * Dane dla HUD pobierane są od lądownika w postaci wektora liczb o ustalonej strukturze.
     */
    void paintEvent(QPaintEvent *);

    void showEvent(QShowEvent *event){ _lander->Reset(); _lander->SetRandPos(); QWidget::showEvent(event); }

    //! Funkcja wywołująca krok lądownika
    void StepScene();

public slots:
    //! Slot wywoływany kiedy pojawia się nowy pomiar
    void NewMeasurementReceived(meas newMeas);

private slots:
    //! Przycisk resetu pozycji i stanu lądownika
    void on_pushButton_clicked();

private:
    Ui::GameWindow *ui;
};

#endif // GAMEWINDOW_HH
