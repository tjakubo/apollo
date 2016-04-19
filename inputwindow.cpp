#include "inputwindow.h"
#include "ui_inputwindow.h"

InputWindow::InputWindow(Hardware *HWlink, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputWindow)
{
    _HWlink = HWlink;
    ui->setupUi(this);
    measOffset.x = measOffset.y = measOffset.z = 0;
    qDebug() << connect(ui->compensateOffset, SIGNAL(clicked()), this, SLOT(updateOffsetTextboxes()));
    qDebug() << connect(ui->resetOffset, SIGNAL(clicked()), this, SLOT(updateOffsetTextboxes()));
    qDebug() << connect(ui->rawEnable, SIGNAL(toggled(bool)), _HWlink, SLOT(setRawDataStatus(bool)));
    qDebug() << connect(_HWlink, SIGNAL(sendRawData(QString)), this, SLOT(updateRawDataTextbox(QString)));
}

InputWindow::~InputWindow()
{
    delete ui;
}

void InputWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pcb(QColor(102, 102, 0), 5, Qt::SolidLine);
    QPen leads(Qt::black, 8, Qt::SolidLine);
    //QPen wires(Qt::red, 2, Qt::SolidLine);
    painter.setPen(pcb);
    int xViz_xCent = ui->xSpinViz->x() + ui->xSpinViz->width()/2;
    int xViz_yCent = ui->xSpinViz->y() + ui->xSpinViz->height()/2;
    painter.translate(xViz_xCent, xViz_yCent);
    int kat = (((double) (actMeas + measOffset).y)/64) * 90;
    if(actMeas.z < 0) kat = 180 - kat;
    painter.rotate(kat);

    painter.translate(-50, 0);
    painter.drawLine(0, 0, 100, 0);

    painter.setPen(leads);
    //painter.translate(0, -13);
    painter.drawLine(3, 6, 3, 20);
    painter.translate(100, 0);
    painter.drawLine(-3, 6, -3, 20);

    painter.resetTransform();
    painter.setPen(pcb);
    int yViz_xCent = ui->ySpinViz->x() + ui->ySpinViz->width()/2;
    int yViz_yCent = ui->ySpinViz->y() + ui->ySpinViz->height()/2;
    painter.translate(yViz_xCent, yViz_yCent);
    kat = (((double) (actMeas + measOffset).x)/64) * 90;
    if(actMeas.z < 0) kat = 180 - kat;
    painter.rotate(kat);
    painter.translate(-20, 0);
    painter.drawLine(0, 0, 40, 0);

    painter.setPen(leads);
    for(int i=0; i<4; i++)
    {
    painter.drawLine(2, 6, 2, 20);
    painter.translate(12, 0);
    }

    painter.resetTransform();
    QPen outline(Qt::black, 2, Qt::SolidLine);
    QPen xForce(Qt::red, 3, Qt::SolidLine);
    QPen yForce(Qt::green, 3, Qt::SolidLine);
    QPen zForce(Qt::blue, 3, Qt::SolidLine);
    int fViz_xCent = ui->forceViz->x() + ui->forceViz->width()/2;
    int fViz_yCent = ui->forceViz->y() + ui->forceViz->height()/2;
    painter.translate(fViz_xCent, fViz_yCent);

    painter.setPen(outline);

    painter.drawLine(-60, 15, -40, -15);
    painter.drawLine(-40, -15, 60, -15);
    painter.drawLine(60, -15, 40, 15);
    painter.drawLine(40, 15, -60, 15);
    painter.drawLine(60, -15, 60, -10);
    painter.drawLine(40, 15, 40, 20);
    painter.drawLine(-60, 15, -60, 20);
    painter.drawLine(-60, 20, 40, 20);
    painter.drawLine(40, 20, 60, -10);

    // mma i led

    painter.setPen(xForce);
    painter.rotate(210);
    int fmeas = (actMeas+measOffset).x;
    painter.drawLine(0, 0, 0, fmeas);
    if(abs(fmeas) > 10)
    {
        painter.drawLine(0, fmeas, 3, (fmeas>0)?(fmeas-5):(fmeas+5));
        painter.drawLine(0, fmeas, -3, (fmeas>0)?(fmeas-5):(fmeas+5));
    }

    painter.setPen(yForce);
    painter.rotate(-120);
    fmeas = (actMeas+measOffset).y;
    painter.drawLine(0, 0, 0, fmeas);
    if(abs(fmeas) > 10)
    {
        painter.drawLine(0, fmeas, 3, (fmeas>0)?(fmeas-5):(fmeas+5));
        painter.drawLine(0, fmeas, -3, (fmeas>0)?(fmeas-5):(fmeas+5));
    }

    painter.setPen(zForce);
    painter.rotate(-90);
    fmeas = (actMeas+measOffset).z;
    painter.drawLine(0, 0, 0, fmeas);
    if(abs(fmeas) > 10)
    {
        painter.drawLine(0, fmeas, 3, (fmeas>0)?(fmeas-5):(fmeas+5));
        painter.drawLine(0, fmeas, -3, (fmeas>0)?(fmeas-5):(fmeas+5));
    }



}

void InputWindow::setMeas(meas newMeas){ actMeas = newMeas; }

void InputWindow::updateBars(meas newMeas){
    ui->_xForce->setValue(actMeas.x + measOffset.x);
    ui->_xForce->setFormat(QString::number(((float) (actMeas.x + measOffset.x))/64, 'f', 2) + " g");
    ui->_yForce->setValue(actMeas.y + measOffset.y);
    ui->_yForce->setFormat(QString::number(((float) (actMeas.y + measOffset.y))/64, 'f', 2) + " g");
    ui->_zForce->setValue(actMeas.z + measOffset.z);
    ui->_zForce->setFormat(QString::number(((float) (actMeas.z + measOffset.z))/64, 'f', 2) + " g");
}

void InputWindow::on_resetOffset_clicked()
{
    measOffset.x = measOffset.y = measOffset.z = 0;
    emit updateOffsetTextboxes();
}

void InputWindow::on_compensateOffset_clicked()
{
    measOffset.x = -1*actMeas.x;
    measOffset.y = -1*actMeas.y;
    measOffset.z = -1*actMeas.z;
    emit updateOffsetTextboxes();
}

void InputWindow::on_applyOffset_clicked()
{
    measOffset.x = ui->xOffset->text().toInt()*64;
    measOffset.y = ui->yOffset->text().toInt()*64;
    measOffset.z = ui->zOffset->text().toInt()*64;
}

void InputWindow::updateOffsetTextboxes()
{
 ui->xOffset->setText(QString::number(((double) measOffset.x)/64));
 ui->yOffset->setText(QString::number(((double) measOffset.y)/64));
 ui->zOffset->setText(QString::number(((double) measOffset.z)/64));
}

void InputWindow::updateRawDataTextbox(QString newRaw)
{
    ui->rawViewer->append(newRaw);
}
