#ifndef RECEIVER_HH
#define RECEIVER_HH

#ifndef HARDWARE_HH
#include "hardware.hh"
#define HARDWARE_HH
#endif

#include <QWidget>

/*! \brief Czysto wirtualna klasa odbiornika nowych pomiarów i/lub danych o pomiarze
 *
 *  Po podaniu źródłowego sprzętu w konstruktorze, zbiera przychodzące dane o nowym pomiarze,
 * nowych danych kalibracji i nowych surowych danych.
 * Posiada puste ciała funkcji odbioru, metoda odbioru samych danych wymaga reimplemetacji w klasie pochodnej.
 */
class Receiver : public QWidget
{
    Q_OBJECT
public:
    //! Konstruktor ze źródłowym obiektem sprzętu
    explicit Receiver(Hardware *sourceHW, QWidget *parent = 0);
    
public slots:
    //! Czysto wirtualna metoda odbioru nowego pomiaru
    virtual void NewMeasurementReceived(meas newMeas) = 0;
    //! Pusta metoda odbioru nowych danych kalibracji
    virtual void NewCalibrationDataReceived(meas calData, int sampleNum);
    //! Pusta metoda nowych surowych danych
    virtual void NewRawDataReceived(QString newRaw);
};

#endif // RECEIVER_H
