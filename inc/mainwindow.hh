#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

/*! \mainpage Wstęp
 *
 * Aplikacja ma formę gry w której sterowanie realizowane jest poprzez obrót podłączonego akcelerometru. <br>
 * Dodatkowo istnieje możliwość wizualizacji i kalibracji wejść oraz dołączenia potencjometru jako sterowania. <br>
 * Przycisk "Pomoc" w oknie gry pokazuje opis interfejsu. <br>
 *
 * Źródła projektu dostępne są pod adresem: http://github.com/tjakubo2/apollo <br>
 * Kod użyty do obsługi sensorów poprzez układ ATmega32 dostępny jest pod adresem: http://github.com/tjakubo2/apollo_hw
 *
 * Projekt realizowany w ramach kursu "Wizualizacja Danych Sensorycznych"
 *
 * Tobiasz Jakubowski <br>
 * WDS 2016
 */

#ifndef HARDWARE_HH
#include "hardware.hh"
#define HARDWARE_HH
#endif

#ifndef INPUTWINDOW_HH
#include "inputwindow.hh"
#define INPUTWINDOW_HH
#endif

#ifndef GAMEWINDOW_HH
#include "gamewindow.hh"
#define GAMEWINDOW_HH
#endif

#include <QMainWindow>
#include <QPainter>
#include <QString>
#include <QDebug>
#include <QTimer>

#define VIEW_FREQ 60 ///< Częstotliwość odrysowywania okienek [Hz]
#define MEAS_FREQ 30 ///< Częstotliwość odpytywania sprzętu o pomiar [Hz]

#define SIM_MEAS false

namespace Ui {
class MainWindow;
}

/*! \brief Klasa zarządzająca oknem głównym i pokazywaniem/chowaniem reszty.
 *
 * Tutaj stworzone zostają timery cykliczne które wywołują update okienek oraz czytanie nowego pomiaru z konfigurowalną częstotliwością. <br>
 * Klasa ta tworzy również obiekt klasy sprzętu który jest później przekazywany innnym okienkom
 * jako m.in. źródło pomiarów. <br>
 * W konstruktorze łączone są również wszystkie sygnały w programie.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    Hardware *_HWlink;         ///< Wskaźnik na (tworzony tutaj) obiekt reprezentujący sprzęt
    InputWindow *_InputWindow; ///< Wskaźnik na (tworzony tutaj) obiekt reprezentujący okno kalibracji
    GameWindow *_GameWindow;   ///< Wskaźnik na (tworzony tutaj) obiekt reprezentujący okno gry

    QTimer *_measTimer;     ///< Timer do cyklicznego wywoływania funkcji pomiaru
    QTimer *_updateTimer;   ///< Timer do cyklicznego wywoływania update okien
    
public:
    //! Konstruktor
    explicit MainWindow(QWidget *parent = 0);
    //! Destruktor
    ~MainWindow();
    
private slots:
    //! Przycisk ukryj/pokaż okno wizualizacji i kalibracji sterowania
    void on_inputWindowButton_clicked();
    //! Przycisk ukryj/pokaż okno gry
    void on_gameWindowButton_clicked();

private:
    Ui::MainWindow *ui; ///< Obiekt z UI z Designera
};

#endif // MAINWINDOW_H
