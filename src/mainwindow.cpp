#include "mainwindow.hh"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(QObject::trUtf8("WDS - Lądownik"));

    // Nowa obiekt zarzadzajacy sprzetem
    _HWlink = new Hardware;

    // Nowy obiekt dla okienka kalibracji
    _InputWindow = new InputWindow(_HWlink);
    _InputWindow->setWindowTitle(QObject::trUtf8("Dane sensoryczne"));

    _GameWindow = new GameWindow(_HWlink);
    _InputWindow->setWindowTitle(QObject::trUtf8("Lądowanie"));


    // TIMER ODSWIEZANIA OKIEN
    QTimer *timer = new QTimer(this);
    qDebug() << connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    qDebug() << connect(timer, SIGNAL(timeout()), _InputWindow, SLOT(UpdateView()));
    qDebug() << connect(timer, SIGNAL(timeout()), _GameWindow, SLOT(update()));
    timer->start(1000/VIEW_FREQ);

    // TIMER ODSWIEAZANIA POMIARU
    QTimer *timer2 = new QTimer(this);
    if(!SIM_MEAS) qDebug() << connect(timer2, SIGNAL(timeout()), _HWlink, SLOT(Measure()));
    else qDebug() << connect(timer2, SIGNAL(timeout()), _HWlink, SLOT(SimMeasure()));
    timer2->start(1000/MEAS_FREQ);

}

MainWindow::~MainWindow()
{
    delete ui;
    _HWlink->close();
    delete _HWlink;
    _InputWindow->close();
    delete _InputWindow;
}

void MainWindow::on_Button_clicked()
{
    _InputWindow->show();
}

void MainWindow::on_pushButton_clicked()
{
    _GameWindow->show();
}

