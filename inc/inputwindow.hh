#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include <QWidget>
#include <QString>
#include <QPainter>

#ifndef HARDWARE_H
#include "hardware.hh"
#define HARDWARE_H
#endif

#ifndef RECEIVER_H
#include "receiver.hh"
#define RECEIVER_H
#endif

namespace Ui {
class InputWindow;
}

/*!
 * \brief Klasa zarządzająca okienkiem kalibracji sterowania
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
    //! Slot wywoływany kiedy pojawia się surowe dane (wkłada do boxa z surowymi danymi)
    void NewRawDataReceived(QString newRaw);

    //! Metoda odświeżająca widgety w okienku - paski ProgressBar itd
    void UpdateView();

private slots:
    void on_resetOffset_clicked();             ///< Klik reset offsetu do wartości domyślnej
    void on_compensateOffset_clicked();        ///< Klik ustawienie offsetu aby była "zerowa" pozycja
    void on_applyOffset_clicked();             ///< Klik ustaw offset z textboxa
    void on_setMaxMeas_clicked();              ///< Klik ustaw aktualny pomiar potensjometru jako max
    void on_resetMaxMeas_clicked();            ///< Klik reset max potensjometru do domyślnej wartości
    void on_setSampleNum_clicked();            ///< Klik ustaw ilość próbek do uśredniania z tetboxa
    void on_rawEnable_clicked(bool checked);   ///< "Radio button" czy chcemy dostawać surowe dane

private:
    Ui::InputWindow *ui; ///< Obiekt z UI z Designera

};

#endif // INPUTWINDOW_H
