#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifndef HARDWARE_H
#include "hardware.h"
#define HARDWARE_H
#endif

#ifndef INPUTWINDOW_H
#include "inputwindow.h"
#define INPUTWINDOW_H
#endif

#include <QMainWindow>
#include <QPainter>
#include <QString>
#include <QDebug>
#include <QTimer>

#define VIEW_FREQ 60 // Odswiezanie okienek [Hz]
#define MEAS_FREQ 30 // Odswiezanie pomiaru [Hz]

namespace Ui {
class MainWindow;
}

// KLASA ZARZADZAJACA GLOWNYM OKNEM
class MainWindow : public QMainWindow
{
    Q_OBJECT
    Hardware *_HWlink;         // (tworzona tutaj) klasa sprzetu
    InputWindow *_InputWindow; // (tworzona tutaj) klasa okna danych
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_Button_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
