#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _HWlink = new Hardware;
    autoLoop = false;
    _InputWindow = new InputWindow;
    qDebug() << connect(_HWlink, SIGNAL(sendMeasurement(meas)), this, SLOT(setMeas(meas)));
    qDebug() << connect(_HWlink, SIGNAL(sendMeasurement(meas)), _InputWindow, SLOT(setMeas(meas)));
    qDebug() << connect(_HWlink, SIGNAL(sendMeasurement(meas)), _InputWindow, SLOT(updateBars(meas)));
    QTimer *timer = new QTimer(this);
    qDebug() << connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    qDebug() << connect(timer, SIGNAL(timeout()), _InputWindow, SLOT(update()));
    timer->start(17);

    QTimer *timer2 = new QTimer(this);
    qDebug() << connect(timer2, SIGNAL(timeout()), _HWlink, SLOT(Measure()));
    timer2->start(34);

}

MainWindow::~MainWindow()
{
    delete ui;
    _HWlink->close();
    delete _HWlink;
    _InputWindow->close();
    delete _InputWindow;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QApplication::quit();
}

void MainWindow::setMeas(meas newMeas)
{
   actMeas = newMeas;
   //update();
   //_InputWindow->update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    static int counter = 0;

    QPainter painter(this);
    QPen pen(Qt::black, 2, Qt::SolidLine);
    painter.setPen(pen);
    painter.drawText(100, 200, QString::number(actMeas.x)+QString::number(actMeas.y)+QString::number(actMeas.z));
    //QWidget::paintEvent(event);
counter++;
   // _HWlink->Measure();
//qDebug() << "repanint mainwindow";
}

void MainWindow::on_Button_clicked()
{
   // autoLoop = !autoLoop;
    _InputWindow->show();
   // _HWlink->Measure();
}
