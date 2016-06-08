#ifndef GAMEWINDOW_HH
#define GAMEWINDOW_HH

#include <QWidget>
#include <QPoint>
#include <QPainter>
#include <QtCore/qmath.h>
#include <QTime>
#include <cstdlib>
#include <QPixmap>
#include <QMouseEvent>

#ifndef HARDWARE_HH
#include "hardware.hh"
#define HARDWARE_HH
#endif

#ifndef RECEIVER_HH
#include "receiver.hh"
#define RECEIVER_HH
#endif

#ifndef PHYSICS_HH
#include "physics.hh"
#define PHYSICS_HH
#endif

#ifndef GAMEOBJECTS_HH
#include "gameobjects.hh"
#define GAMEOBJECTS_HH
#endif

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
    //! Konstruktor parametryczny
    explicit GameWindow(Hardware *HWlink);

    //! Destruktor
    ~GameWindow();

    //! Metoda rysująca HUD oraz wywołująca rysowanie terenu i lądownika oraz krok sceny
    void paintEvent(QPaintEvent *);

    //! Event wywołujący się po otwarciu okna
    void showEvent(QShowEvent *event);

    //! Event wywołujacy się przy ruszaniu myszką z wciśniętym LMB (do przeciągania statku)
    void mouseMoveEvent(QMouseEvent* event);

    //! Funkcja wywołująca krok lądownika (reszta fizyki się dzieje od niego)
    void StepScene();

public slots:
    //! Slot wywoływany kiedy pojawia się nowy pomiar
    void NewMeasurementReceived(meas newMeas);

private slots:
    //! Przycisk resetu pozycji i stanu lądownika
    void on_resetButton_clicked();
    //! Slider współczynnika ilości cząsteczek
    void on_partSlider_valueChanged(int value);
    //! Slider siły grawitacji
    void on_gravSlider_valueChanged(int value);
    //! Przycik pokazywania/chowania okna pomocy
    void on_helpButton_clicked();

private:
    Ui::GameWindow *ui;
};

#endif // GAMEWINDOW_HH
