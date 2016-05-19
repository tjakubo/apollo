#include "gamewindow.hh"
#include "ui_gamewindow.h"

GameWindow::GameWindow(Hardware *HWlink) :
    Receiver(HWlink),
    ui(new Ui::GameWindow)
{
    _terr = new Terrain(0, 1000, 4, 10, 0.15);
    _lander = new Ship();
    _lander->SetPosRand(0.25, 0.8);
    ui->setupUi(this);

    QTime time = QTime::currentTime();
            qsrand((uint)time.msec());
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::NewMeasurementReceived(meas newMeas)
{
    int tilt = (((double) newMeas.y)/64) * 90;
    if(newMeas.z < 0) tilt = 180 - tilt;

    int set = (((double) newMeas.x)/64) * -90;
    if(newMeas.z < 0) set = 180 - set;
    _lander->Steer(((double) set)/100, ((double) tilt)/100);
    //qDebug() << ((double) set)/100 << " : " << ((double) tilt)/100;
}

Ship::Ship()
{
    _maxThrust = 100;
    _maxTorq = 1;

    _xPos = _yPos = _ang = 0;
    _xVel = _yVel = _angVel = 0;

    _legAngle = 45;
    _legLength = 30;
    _fragileRadius = 15;
}

void Ship::SetPosRand(double maxCenterOffsetPerc, double minHeightPerc)
{
    double w=1000, h=750;
    _xPos = rand()%((int) (w*maxCenterOffsetPerc*2)) + (w/2 -w*maxCenterOffsetPerc);
    _yPos = rand()%((int) (h*(1-minHeightPerc)));
    //qDebug() << Pos();

}

void Ship::SetPos(QPoint newPos)
{
    _xPos = newPos.x();
    _yPos = newPos.y();
}

void Ship::Steer(double newThrustPerc, double newTorqPerc)
{
    if(newThrustPerc > 1) newThrustPerc = 1;
    if(newThrustPerc < 0) newThrustPerc = 0;
    if(newTorqPerc > 1) newTorqPerc = 1;
    if(newTorqPerc < -1) newTorqPerc = -1;
    _currThrustPerc = newThrustPerc;
    _currTorqPerc = newTorqPerc;
    //qDebug() << _currThrustPerc;
}

void Ship::Step()
{
    double deltaTime = (double) Timer.restart()/1000;

    static bool firstRun = true;
    if(firstRun){ deltaTime = 0.001; firstRun = false; }

    _xPos += _xVel*deltaTime;
    _yPos += _yVel*deltaTime;
    _ang += _angVel*deltaTime;

    _xVel += _xAcc*deltaTime;
    _yVel += _yAcc*deltaTime;
    _angVel += _torq*deltaTime;

    _torq = _currTorqPerc * _maxTorq;
    double currThrust = _currThrustPerc * _maxThrust;
    _xAcc = currThrust * qSin(_ang);
    _yAcc = -1 * currThrust * qCos(_ang);
    _yAcc +=5;
    //qDebug() << _currThrustPerc << " : " << _maxThrust << " : " << currThrust;

}

void GameWindow::StepShip()
{
    _lander->Step();
    double ang = _lander->AngleRad();
    double legOffset = 45.0*(2.0*3.1416)/360.0;
    QPoint leg1(30*qCos(ang+legOffset), 30*qSin(ang+legOffset)), leg2(30*qCos(ang+3*legOffset), 30*qSin(ang+3*legOffset));
    leg1 += _lander->Pos(); leg2 += _lander->Pos();

    int leg1margin, leg2margin, shipmargin;
    //qDebug() << leg1.y() << " : " << _terr->ElevAtX(leg1.x());
    if((leg1margin = ((750-leg1.y()) - _terr->ElevAtX(leg1.x()))) < 0)
    {
        _lander->Stop();
        _lander->SetPos(QPoint(_lander->Pos().x(), _lander->Pos().y()+leg1margin));
    }
    if((leg2margin = ((750-leg2.y()) - _terr->ElevAtX(leg2.x()))) < 0)
    {
        _lander->Stop();
        _lander->SetPos(QPoint(_lander->Pos().x(), _lander->Pos().y()+leg2margin));
    }
    if((shipmargin = ((750-_lander->Pos().y()) - _terr->ElevAtX(_lander->Pos().x()))) < 15)
        _lander->SetPosRand(0.25, 0.8);
    qDebug() << leg1margin << " : " << leg2margin;

}

QPoint Ship::Pos()
{
    return QPoint(_xPos, _yPos);
}

double Ship::Angle()
{
    return (_ang*360)/(3.1416*2);
}

double Ship::AngleRad()
{
    return _ang;
}


void Ship::Stop()
{
    _xVel = _yVel = _angVel = 0;

}

Terrain::Terrain(int xMin, int xMax, int vertCountMin, int vertCountMax, double dYmax)
{
   int vertCount = (rand()%(vertCountMax - vertCountMin)) + vertCountMin;

   int xCurr = 0;
   int yCurr = rand()%( (int)((xMax - xMin)*dYmax)) - dYmax/2;
   _verts.push_back(QPoint(xCurr, yCurr));
   for(int i=0; i<vertCount; i++)
   {
       int dX = rand()%1000;
       xCurr += dX;
       int yMax = dYmax * dX;
       int yCurr = rand()%(2*yMax) - yMax;
       _verts.push_back(QPoint(xCurr, yCurr));
   }
   double normCoeff = (double) xMax/_verts[_verts.size()-1].x();
   for(int i=0; i<_verts.size(); i++)
   {
       _verts[i].rx() = _verts[i].x()*normCoeff;
       _verts[i].ry() = _verts[i].y()*normCoeff;
   }

}

int Terrain::LowestElev()
{
    int lowest = 0;
    for(int i=0; i<_verts.size(); i++)
    {
        if(i==0) lowest = _verts[i].y();
        if(_verts[i].y() < lowest)
            lowest = _verts[i].y();
    }
    return lowest;
}

int Terrain::ElevAtX(int xPos)
{
    if(xPos < 0) xPos = 10;
    if(xPos > 1000) xPos = 990;
    static int lowElev = LowestElev();
    int i = 1;
    while(_verts[i].x() < xPos) i++;
    double dY = (_verts[i].y() - _verts[i-1].y())/(double) (_verts[i].x() - _verts[i-1].x());
    return (xPos - _verts[i-1].x())*dY + _verts[i-1].y() - LowestElev()*2;
}

double Terrain::TiltAtX(int xPos)
{
    if(xPos < 0) xPos = 1;
    if(xPos > 1000) xPos = 999;
    int i = 0;
    while(_verts[i].x() < xPos && i < _verts.size()) i++;
    double dY = _verts[i].y() - _verts[i-1].y();
    return qAtan(dY);
}

void GameWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen line(Qt::black, 3, Qt::SolidLine);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(line);
    //qDebug() << _terr->LowestElev();
    painter.translate(0, 750+(_terr->LowestElev()*2));
    painter.rotate(0);

    for(int i=0; i<(_terr->Vec().size()-1); i++)
    {
        painter.drawLine(QPoint(_terr->Vec()[i].x(), -1*_terr->Vec()[i].y()), QPoint(_terr->Vec()[i+1].x(), -1*_terr->Vec()[i+1].y()));
    }
    //qDebug() << QWidget::size();
    //qDebug() << _lander->Pos();
    painter.resetTransform();
    painter.translate(_lander->Pos());
    painter.rotate(_lander->Angle());
    painter.drawPoint(15, 0);
    painter.drawPoint(-15, 0);
    painter.drawPoint(0, -15);
    //painter.drawEllipse(QPoint(0, 0), 15, 15);
    painter.rotate(45);
    painter.drawLine(0, 0, 0, 30);
    painter.rotate(-90);
    painter.drawLine(0, 0, 0, 30);
    StepShip();
    //_lander->Step();

    //painter.resetTransform();
    //painter.translate(0, 750);
    //for(int i=10; i<990; i+=10)
    //{
    //    painter.drawPoint(i, -1*_terr->ElevAtX(i));
    //}
}

const QVector<QPoint> Terrain::Vec(){ return _verts; }

void GameWindow::on_pushButton_clicked()
{
    _lander->SetPosRand(0.25, 0.8);
    _lander->Stop();
}
