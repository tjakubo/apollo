#include "inputwindow.h"
#include "ui_inputwindow.h"

InputWindow::InputWindow(Hardware *HWlink) :
        Reciever(HWlink),
    ui(new Ui::InputWindow)
{
        ui->setupUi(this);
    _HWlink = HWlink;
    _HWlink->SetCal(_HWlink->GetCal());


    qDebug() << connect(_HWlink, SIGNAL(NewMeasurement(meas)), this, SLOT(NewMeasurementRecieved(meas)));
    qDebug() << connect(_HWlink, SIGNAL(NewCalibrationData(meas,int)), this, SLOT(NewCalibrationDataRecieved(meas,int)));
    qDebug() << connect(_HWlink, SIGNAL(NewRawData(QString)), this, SLOT(NewRawDataRecieved(QString)));
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
    QPen led(Qt::red, 3, Qt::SolidLine);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(pcb);
    int xViz_xCent = ui->xSpinViz->x() + ui->xSpinViz->width()/2;
    int xViz_yCent = ui->xSpinViz->y() + ui->xSpinViz->height()/2;
    painter.translate(xViz_xCent, xViz_yCent);
    int kat = (((double) _actMeas.y)/64) * 90;
    if(_actMeas.z < 0) kat = 180 - kat;
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
    kat = (((double) _actMeas.x)/64) * 90;
    if(_actMeas.z < 0) kat = 180 - kat;
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

    // dorysowac mma i led

    painter.setPen(xForce);
    painter.rotate(210);
    int fmeas = (_actMeas).x;
    painter.drawLine(0, 0, 0, fmeas);
    if(abs(fmeas) > 10)
    {
        painter.drawLine(0, fmeas, 3, (fmeas>0)?(fmeas-5):(fmeas+5));
        painter.drawLine(0, fmeas, -3, (fmeas>0)?(fmeas-5):(fmeas+5));
    }

    painter.setPen(yForce);
    painter.rotate(-120);
    fmeas = (_actMeas).y;
    painter.drawLine(0, 0, 0, fmeas);
    if(abs(fmeas) > 10)
    {
        painter.drawLine(0, fmeas, 3, (fmeas>0)?(fmeas-5):(fmeas+5));
        painter.drawLine(0, fmeas, -3, (fmeas>0)?(fmeas-5):(fmeas+5));
    }

    painter.setPen(zForce);
    painter.rotate(-90);
    fmeas = (_actMeas).z;
    painter.drawLine(0, 0, 0, fmeas);
    if(abs(fmeas) > 10)
    {
        painter.drawLine(0, fmeas, 3, (fmeas>0)?(fmeas-5):(fmeas+5));
        painter.drawLine(0, fmeas, -3, (fmeas>0)?(fmeas-5):(fmeas+5));
    }



}

void InputWindow::NewMeasurementRecieved(meas newMeas) { _actMeas = newMeas; }

void InputWindow::NewRawDataRecieved(QString newRaw)
{
    ui->rawViewer->append(newRaw);
}

void InputWindow::UpdateView()
{
    ui->_xForce->setValue(_actMeas.x);
    ui->_xForce->setFormat(QString::number(((float) _actMeas.x )/INT_1G, 'f', 2) + " g");
    ui->_yForce->setValue(_actMeas.y);
    ui->_yForce->setFormat(QString::number(((float) _actMeas.y )/INT_1G, 'f', 2) + " g");
    ui->_zForce->setValue(_actMeas.z);
    ui->_zForce->setFormat(QString::number(((float) _actMeas.z )/INT_1G, 'f', 2) + " g");

    int pMax = _actCalData.p;
    ui->pValue->setFormat(QString::number(((float) _actMeas.p*100)/_actCalData.p, 'f', 1) + " %");
    ui->pValue->setValue( ( ( (float) _actMeas.p )/pMax )*100);
    this->update();
}

void InputWindow::NewCalibrationDataRecieved(meas calData, int sampleNum)
{
    _actCalData = calData;
    _actSampleNum = sampleNum;
    ui->xOffset->setText(QString::number(((float) calData.x)/INT_1G));
    ui->yOffset->setText(QString::number(((float) calData.y)/INT_1G));
    ui->zOffset->setText(QString::number(((float) calData.z)/INT_1G));

    if(sampleNum == 1) ui->sampleNum->setText(QString::number(sampleNum) + QObject::trUtf8(" (wył)"));
    else ui->sampleNum->setText(QString::number(sampleNum));
}

void InputWindow::on_resetOffset_clicked()
{
    meas newCal = _HWlink->GetCal();
    newCal.x = newCal.y = newCal.z = 0;
    _HWlink->SetCal(newCal);
}

void InputWindow::on_compensateOffset_clicked()
{
    meas newCal = _HWlink->GetCal();
    newCal.x = -1*(_actMeas.x - _actCalData.x);
    newCal.y = -1*(_actMeas.y - _actCalData.y);
    newCal.z = -1*(_actMeas.z - _actCalData.z) + INT_1G;
    _HWlink->SetCal(newCal);
}

void InputWindow::on_applyOffset_clicked()
{
    meas newCal = _HWlink->GetCal();
    newCal.x = ui->xOffset->text().toInt()*64;
    newCal.y = ui->yOffset->text().toInt()*64;
    newCal.z = ui->zOffset->text().toInt()*64;
    _HWlink->SetCal(newCal);
}

void InputWindow::on_setMaxMeas_clicked()
{
    meas newCal = _HWlink->GetCal();
    newCal.p = _actMeas.p;
    _HWlink->SetCal(newCal);
}

void InputWindow::on_resetMaxMeas_clicked()
{
    meas newCal = _HWlink->GetCal();
    newCal.p = 1024;
    _HWlink->SetCal(newCal);
}
void InputWindow::on_setSampleNum_clicked()
{
    int newSampleNum = ui->sampleNum->text().toInt();
    _HWlink->SetSampleNum(newSampleNum);
}

void InputWindow::on_rawEnable_clicked(bool checked)
{
    _HWlink->SetRawDataStatus(checked);
}
