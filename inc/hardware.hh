#ifndef HARDWARE_HH
#define HARDWARE_HH

#include <QWidget>
#include <QString>
#include <QDebug>
#include <QTimer>
#include <boost/asio.hpp>
#include <thread>

#ifndef BLOCKING_READER_HH
#include "blocking_reader.hh"
#define BLOCKING_READER_HH
#endif

#define MEAS_PORT "/dev/ttyS0" ///< Port jakim się komunikujemy ze sprzętem
#define SAMPLE_NUM_MAX 30      ///< Maksymalna liczba próbek w uśrednianiu pomiaru
#define INT_1G 64              ///< Wartość pomiaru przyspieszenia równa 1g
#define TIMEOUT_MS 50          ///< Ile milisekund mamy czekać na nowy znak z pomiaru

#define SIM_MEAS_STR "b 20 -15 64 512 1 e" ///< "Pomiar" przy symulacji odbioru pomiaru

//! Struktura przechowująca pojedynczy pomiar
struct meas
{
    int x, y, z; ///< Pomiary przyspieszeń osi akcelerometru
    int p;       ///< Pomiar napięcia na potencjometrze

    //! Konstruktor bezparametryczny
    meas();
    //! Konstruktor parametryczny z wszystkimi danymi
    meas(int nx, int ny, int nz, int np);

    //! Metoda dodająca do pomiaru wartości przyspieszeń drugiego pomiaru
    void OffsetXYZ(meas offset);
};

//! Dodawanie 2 pomiarów
meas operator+ (meas m1, meas m2);
//! Dzielenie pomiaru przez liczbę (każde pole)
meas operator/ (meas m, int div);


/*!
 * \brief Klasa obsługująca połączenie ze sprzętem - odczyt, weryfikacja/kalibracja danych, przesyłanie dalej.
 *
 * Realizuje połączenie poprzez odczyt z portu szeregowego funkcjami bilbioteki Boost. Na to nałożona
 * została funkcjonalność zewnętrznej biblioteki blocking_reader.hh Kevina Goddena. <br>
 * Efekt to odczyt synchroniczny z funkcją timeoutu. <br>
 * Oprócz tego klasa sprawdza sumę kontrolną pomiaru i nakłąda żądane przetwarzanie (kalibracja, uśrednianie).
 *
 * Klasa może otrzymywać dane z dowolnego źródła (byle były zgodne z formatem opisanym przy metodzie Measure() ),
 * ale docelowo źródłem pomiarów jest akcelerometr MMA7455 z którym komunikuje się mikroprocesor AtMega32 po magistrali
 * I2C, a dalej z komputerem poprzez moduł UART i układ FTDI. Kod źródłowy programu wgranego do procesora jest dostępny pod adresem:
 * www.github.com/tjakubo2/apollo_hw.
 */
class Hardware: public QWidget{
    Q_OBJECT


    boost::asio::io_service _ios;  ///< Obiekt do obsługi komunikacji
    boost::asio::serial_port _sp;  ///< Obiekt do obsługi portu szeregowego
    //QTimer _timer;
    blocking_reader _reader;       ///< Obiekt realizujący odczyt z timeoutem


    bool _rawDataSent;   ///< Czy wysyłane są surowe dane?
    meas _measCal;       ///< Wartość kalibracji pomiaru (offset, max 4. dana)
    int _avgSampleNum;   ///< Ilość ostatnich pomiarów do uśrednienia wraz z nowym

    //! Metoda przetwarzająca pomiar z argumentu
    meas Process(meas measurement);

public:
    //! Konstruktor bezparametrczny
    Hardware(QWidget *parent = nullptr);


    void SetCal(meas newCal);               ///< Ustaw nowe dane do kalibracji
    meas GetCal();                          ///< Pobierz aktualne dane o kalibracji
    void SetSampleNum(int newSampleNum);    ///< Ustaw nową ilość próbek do uśredniania
    int GetSampleNum();                     ///< Pobierz aktualną ilosć próbek do uśredniania
    void SetRawDataStatus(bool rawSent);    ///< Ustaw czy powinno wysyłać surowe dane

public slots:
    void Measure();     ///< Metoda rozpoczynająca pobieranie nowego pomiaru
    void SimMeasure();  ///< Metoda symulująca otrzymanie nowego pomiaru

signals:
    void NewMeasurement(meas measurement);                    ///< Sygnał - otrzymno nowy pomiar
    void NewRawData(QString rawData);                         ///< Sygnał - otrzymano nowe surowe dane
    void NewCalibrationData(meas newCal, int newSampleNum);   ///< Sygnał - otrzymano nowe dane kalibracji

};

#endif // HARDWARE_H
