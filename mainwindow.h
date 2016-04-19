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
#include "qpainter.h"
#include "qstring.h"
#include <qdebug.h>
#include <qtimer.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void paintEvent(QPaintEvent *);
    void closeEvent(QCloseEvent *);

    //meas actMeas;
    //bool autoLoop;
    Hardware *_HWlink;
    InputWindow *_InputWindow;

    //public slots:
    //void setMeas(meas measurement);
    
private slots:
    void on_Button_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
