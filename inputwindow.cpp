#include "inputwindow.h"
#include "ui_inputwindow.h"

InputWindow::InputWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputWindow)
{
    ui->setupUi(this);
}

InputWindow::~InputWindow()
{
    delete ui;
}

void InputWindow::paintEvent(QPaintEvent *)
{
   //ui->_xForce->setValue(actMeas.x);
    //ui->_xForce->
//qDebug() << "repainting inputwindow";

}

void InputWindow::setMeas(meas newMeas){ actMeas = newMeas; }

void InputWindow::updateBars(meas newMeas){
    ui->_xForce->setValue(actMeas.x);
    ui->_xForce->setFormat(QString::number(((float) actMeas.x)/64, 'f', 2) + " g");
    ui->_yForce->setValue(actMeas.y);
    ui->_yForce->setFormat(QString::number(((float) actMeas.y)/64, 'f', 2) + " g");
    ui->_zForce->setValue(actMeas.z);
    ui->_zForce->setFormat(QString::number(((float) actMeas.z)/64, 'f', 2) + " g");
}
