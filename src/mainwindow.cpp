#include "mainwindow.hh"
#include "ui_mainwindow.h"

/*!
 * Tworzy obiekty sprzętu i okien.
 *
 * Łączy sygnały cyklicznych timerów z odpowiednimi funkcjami i sprawdza czy zostały odpowiednio połączone.
 */
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
    _GameWindow->setWindowTitle(QObject::trUtf8("Lądowanie"));

    bool connectOK = true;

    // TIMER ODSWIEZANIA OKIEN
    _updateTimer = new QTimer(this);
    connectOK = connectOK && connect(_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    connectOK = connectOK && connect(_updateTimer, SIGNAL(timeout()), _InputWindow, SLOT(UpdateView()));
    connectOK = connectOK && connect(_updateTimer, SIGNAL(timeout()), _GameWindow, SLOT(update()));
    _updateTimer->start(1000/VIEW_FREQ);

    // TIMER ODSWIEAZANIA POMIARU
    _measTimer = new QTimer(this);
    if(!SIM_MEAS)
        connectOK = connectOK && connect(_measTimer, SIGNAL(timeout()), _HWlink, SLOT(Measure()));
    else
        connectOK = connectOK && connect(_measTimer, SIGNAL(timeout()), _HWlink, SLOT(SimMeasure()));
    _measTimer->start(1000/MEAS_FREQ);

    if(!connectOK)
        qDebug() << "Blad podczas łączenia sygnałów odświeżania/pomiaru";

}

MainWindow::~MainWindow()
{
    delete ui;
    _HWlink->close();
    delete _HWlink;
    _InputWindow->close();
    delete _InputWindow;
}

void MainWindow::on_inputWindowButton_clicked()
{
    if(_InputWindow->isHidden())
            _InputWindow->show();
    else
        _InputWindow->hide();
}

void MainWindow::on_gameWindowButton_clicked()
{
    if(_GameWindow->isHidden())
            _GameWindow->show();
    else
        _GameWindow->hide();
}

