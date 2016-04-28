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

// KLASA ZARZADZAJACA OKIENKIEM PODGLADU DANYCH I KALIBRACJI
class InputWindow : public Reciever
{
    Q_OBJECT
    meas _actMeas;      // ostatnia wartosc pomiaru
    meas _actCalData;   // ostatnie dane kalibracji
    int _actSampleNum;  // ostatnia ilosc probek do usredniania

    // Polaczenie ze sprzetem
    // (potrzebne do odczytu/zmiany kalibracji)
    Hardware *_HWlink;

public:

    explicit InputWindow(Hardware *HWlink);
    ~InputWindow();
    void paintEvent(QPaintEvent *);

public slots:
    // Sloty reagujace na nowe dane
    void NewMeasurementRecieved(meas newMeas);
    void NewCalibrationDataRecieved(meas calData, int sampleNum);
    void NewRawDataRecieved(QString newRaw);

    // Odswiez elementy pasywne widoku
    void UpdateView();

private slots:
    // Sloty z designera
    void on_resetOffset_clicked();
    void on_compensateOffset_clicked();
    void on_applyOffset_clicked();
    void on_setMaxMeas_clicked();
    void on_resetMaxMeas_clicked();
    void on_setSampleNum_clicked();
    void on_rawEnable_clicked(bool checked);

private:
    Ui::InputWindow *ui;

};

#endif // INPUTWINDOW_H
