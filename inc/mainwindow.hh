#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifndef HARDWARE_H
#include "hardware.hh"
#define HARDWARE_H
#endif

#ifndef INPUTWINDOW_H
#include "inputwindow.hh"
#define INPUTWINDOW_H
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

#define VIEW_FREQ 60 // Odswiezanie okienek [Hz]
#define MEAS_FREQ 30 // Odswiezanie pomiaru [Hz]

#define SIM_MEAS false

namespace Ui {
class MainWindow;
}

// KLASA ZARZADZAJACA GLOWNYM OKNEM
class MainWindow : public QMainWindow
{
    Q_OBJECT
    Hardware *_HWlink;         // (tworzona tutaj) klasa sprzetu
    InputWindow *_InputWindow; // (tworzona tutaj) klasa okna danych
    GameWindow *_GameWindow;   // (towrzona tutaj( klasa okna gry
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_Button_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
