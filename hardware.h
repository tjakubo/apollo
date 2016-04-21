#ifndef HARDWARE_H
#define HARDWARE_H

#include <QWidget>
#include <QString>
#include <QDebug>
#include <boost/asio.hpp>
#include <thread>

#define MEAS_PORT "/dev/ttyS0"
#define SAMPLE_NUM_MAX 30
#define INT_1G 64

// STRUKTURA ZAWIERAJACA JEDEN POMIAR
struct meas
{
    int x, y, z; // pomiary przyspieszen osi akcelerometru
    int p;       // pomiar napiecia potencjometru

    meas();
    meas(int nx, int ny, int nz, int np);
    void OffsetXYZ(meas offset);
};
// //

// Dzialania na pomiarze
meas operator+ (meas m1, meas m2);
meas operator/ (meas m, int div);
// //

// KLASA OBSLUGUJACA SPRZET - POMIAR, PRZETWARZANIE POMIARU
class Hardware: public QWidget{
    Q_OBJECT

    // Zmienne do komunikacji poprzez port szeregowy
    boost::asio::io_service _ios;
    boost::asio::serial_port _sp;

    // Zmienne do przetwarzania pomiaru
    bool _rawDataSent;   // czy powinna wysylac surowe dane
    meas _measCal;       // wartosci do kalibracji pomiaru
    int _avgSampleNum;   // ilosc probek do usredniania pomiaru
    meas Process(meas measurement);


public:
    Hardware(QWidget *parent = nullptr);

    // Metody dostepu do wartosci kalibracji
    void SetCal(meas newCal);
    meas GetCal();
    void SetSampleNum(int newSampleNum);
    int GetSampleNum();

    // Metoda wlacz/wylacz przesyl surowych danych
    void SetRawDataStatus(bool rawSent);

public slots:
    void Measure(); // Metoda rozpoczynajaca nowy pomiar

signals:
    void NewMeasurement(meas measurement);
    void NewRawData(QString rawData);
    void NewCalibrationData(meas newCal, int newSampleNum);

};

#endif // HARDWARE_H
