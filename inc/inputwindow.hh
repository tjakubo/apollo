#ifndef INPUTWINDOW_HH
#define INPUTWINDOW_HH

#include <QWidget>
#include <QString>
#include <QPainter>

#ifndef HARDWARE_HH
#include "hardware.hh"
#define HARDWARE_HH
#endif

#ifndef RECEIVER_HH
#include "receiver.hh"
#define RECEIVER_HH
#endif

namespace Ui {
class InputWindow;
}

/*!
 * \brief Klasa zarządzająca okienkiem kalibracji sterowania.
 *
 * Pozwala na ustawienie wszystkich parametrów kalibracji (które działają dla wszystkich odbiorników
 * danych z powiązanego sprzętu), sprawdzenie odbieranych surowych danych oraz wizualnie prezentuje
 * stan czujników.
 */
class InputWindow : public Receiver
{
    Q_OBJECT
    meas _actMeas;      ///< Ostatnia wartosc pomiaru
    meas _actCalData;   ///< Ostatnie dane kalibracji
    int _actSampleNum;  ///< Ostatnia ilosc próbek do uśredniania

    //! Wskaźnik na obiekt reprezentujacy sprzęt od którego pobieramy dane
    Hardware *_HWlink;

public:

    //! Konstruktor parametryczny - potrzebuje wskaźnika na kojarzoną klasę sprzętu
    explicit InputWindow(Hardware *HWlink);
    //! Destruktor
    ~InputWindow();
    //! Metoda malująca wizualizacje danych sensorycznych - rokład sił, orientację akcelerometru etc
    void paintEvent(QPaintEvent *);

public slots:
    //! Slot wywoływany kiedy pojawia się nowy pomiar (nowe dane do wyświetlenia)
    void NewMeasurementReceived(meas newMeas);
    //! Slot wywoływany kiedy pojawią się nowe dane kalibracji (udate textboxów od kalibracji)
    void NewCalibrationDataReceived(meas calData, int sampleNum);
    //! Slot wywoływany kiedy pojawią się surowe dane (wkłada do boxa z surowymi danymi)
    void NewRawDataReceived(QString newRaw);

    //! Metoda odświeżająca widgety w okienku - paski ProgressBar itd
    void UpdateView();

private slots:
    void on_resetOffset_clicked();             ///< Przycisk resetu offsetu do wartości domyślnej
    void on_compensateOffset_clicked();        ///< Przycisk ustawienia offsetu aby była "zerowa" pozycja
    void on_applyOffset_clicked();             ///< Przycisk ustawienia offset z textboxa
    void on_setMaxMeas_clicked();              ///< Przycisk ustawienia aktualnego pomiaru potencjometru jako max
    void on_resetMaxMeas_clicked();            ///< Przycisk resetu max pomiaru potencjometru do domyślnej wartości
    void on_setSampleNum_clicked();            ///< Przycisk ustawienia ilość próbek do uśredniania z textboxa
    void on_rawEnable_clicked(bool checked);   ///< Przycisk "RadioButton" ustawienia czy chcemy dostawać surowe dane

private:
    Ui::InputWindow *ui; ///< Obiekt z UI z Designera

};

#endif // INPUTWINDOW_H
