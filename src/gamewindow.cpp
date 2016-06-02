#include "gamewindow.hh"
#include "ui_gamewindow.h"

void PhysicsObj::Step(double dt)
{
    // jak 1 uruchomienie to dt=1ms
    double deltaTime = (double) _timer.restart()/1000;
    if(_firstRun){ deltaTime = 0.001; _firstRun = false;}
    if(dt > 0) deltaTime = dt; // podstaw argument jak sie da

    _pos = _pos + _vel*deltaTime;
    _vel = _vel + _acc*deltaTime + _gravity*deltaTime;

}

void PhysicsObj::AddForce(pos2d aForce)
{
    _acc = _acc + aForce;
}

void PhysicsObj::RotateVel(double angDeg)
{
    double angRad = qDegreesToRadians(angDeg);
    pos2d oldVel = _vel;
    _vel.x = oldVel.x*qCos(angRad) - oldVel.y*qSin(angRad);
    _vel.y = oldVel.x*qSin(angRad) + oldVel.y*qCos(angRad);
}

PhysicsObj::PhysicsObj(): _pos(), _vel(), _acc(), _gravity(), _firstRun(true) {}

PhysicsObj::PhysicsObj(pos2d initPos, pos2d initVel, pos2d initGrav):
    _pos(initPos),
    _vel(initVel),
    _gravity(initGrav),
    _firstRun(true)
{}

void Terrain::Draw(QPainter *painter)
{
    QPolygon terrPoly; // wielobok do rysowania terenu

    painter->save();

    QBrush terrBrush(QColor::fromRgb(200, 200, 200));
    QPen terrPen(QColor::fromRgb(200, 200, 200));
    painter->setPen(terrPen);
    painter->setBrush(terrBrush);
    for(int i=0; i<_verts.size(); i++)
        terrPoly << Trans(_verts[i]); // normalnie teren ma postac przed przeksztalceniem do ukladu Qt!
    terrPoly << QPoint(1000, 650) << QPoint(0, 650);
    painter->drawPolygon(terrPoly);

    painter->restore();
    painter->resetTransform();
}

void Ship::Draw(QPainter *painter)
{        
    static QPixmap shipImage(":/img/lander_ok.png");

    if(_state > 0) // jak nie OK to zmien obrazek
    {
        switch (_state) {
        case 1:
            shipImage.load(":/img/lander_destroyed.png");
            break;
        case 2:
            shipImage.load(":/img/lander_damaged_left.png");
            break;
        case 3:
            shipImage.load(":/img/lander_damaged_right.png");
            break;
        default:
            shipImage.load(":/img/lander_ok.png"); // jak status wiekszy niz zly to zmien na OK
            _state = 0;
            break;

        }
    }
    painter->translate(Pos().Point());
    painter->rotate(Pos().AngDeg());
    painter->scale(0.5, 0.5);
    // normalnie rysuje od lewego gornego rogu to trzeba przesunac na srodek
    painter->drawPixmap(QPoint(-1*shipImage.width()/2, -1*shipImage.height()/2), shipImage);
    painter->resetTransform();

    // odswiez czasteczki
    for(unsigned int i=0; i<_spawnedParticles.size(); i++)
    {
        if(_spawnedParticles[i]->Expired())
        {
            delete _spawnedParticles[i];
            _spawnedParticles.erase(_spawnedParticles.begin() + i);
        }
        else
            _spawnedParticles[i]->Draw(painter);
    }
}




GameWindow::GameWindow(Hardware *HWlink) :
    Receiver(HWlink),
    ui(new Ui::GameWindow)
{
    _terr = new Terrain(0, 1000, 4, 10, 0.15);
    _lander = new Ship(_terr);
    ui->setupUi(this);

    //nowy seed randa
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::NewMeasurementReceived(meas newMeas)
{
    int tilt = (((double) newMeas.y)/64) * 90; // WYCHYLENIE NA BOK
    if(newMeas.z < 0) tilt = 180 - tilt;       // czerwona dioda w swoja strone

    int set = (((double) newMeas.x)/64) * -90; // WYCHYLENIE W PRZOD
    if(newMeas.z < 0) set = 180 - set;         // w przeciwna niz czerwona dioda
    _lander->Steer(((double) set)/100, ((double) tilt)/100);
}

Ship::Ship(Terrain *terr):
    PhysicsObj(pos2d(0, 0, 0), pos2d(0, 0, 0), pos2d(0, 5, 0)),
    _terr(terr)
{
    _maxThrust = 60;
    _maxTorq = 1;
    _state = 0;

    _maxFuelCap = 1000;
    _maxFuelConsPerSec = 5;
    _currFuel = _maxFuelCap;

    _legAngle = 45;
    _legLength = 50;
    _legMaxImpactVel = 40;
    _fragileRadius = 30;
    _leg1coll = _leg2coll = false;
    SetRandPos();
}

void Ship::Steer(double newThrustPerc, double newTorqPerc)
{
    if(newThrustPerc > 1) newThrustPerc = 1;
    if(newThrustPerc < 0) newThrustPerc = 0;
    if(newTorqPerc > 1) newTorqPerc = 1;
    if(newTorqPerc < -1) newTorqPerc = -1;
    _currThrustPerc = newThrustPerc;
    _currTorqPerc = newTorqPerc;
    double actThrust = _currThrustPerc*_maxThrust;

    // jesli zero paliwa to zero ale currThrust zostaje na zadanej!
    if(_currFuel == 0) actThrust = 0;
    double actTorque = _currTorqPerc*_maxTorq;


    SetAcc(pos2d(actThrust * qSin(Pos().AngRad()), -1*actThrust * qCos(Pos().AngRad()), actTorque));
}

void PhysicsObj::Stop()
{
    _vel = pos2d(0, 0, 0);
    _timer.restart();
}

void Ship::Step(double dt)
{
    qDebug() << Tilt();

    _currFuel -= _maxFuelConsPerSec*_currThrustPerc;
    if(_currFuel < 0) _currFuel = 0;
    double ang = Pos().AngRad();
    double legOffset = _legAngle*(2.0*3.1416)/360.0;
    QPoint leg1(_legLength*qCos(ang+legOffset), _legLength*qSin(ang+legOffset)),
           leg2(_legLength*qCos(ang+3*legOffset), _legLength*qSin(ang+3*legOffset));
    QPoint shipCenter = Trans(Pos().Point());
    leg1 += Pos().Point(); leg2 += Pos().Point();
    leg1 = Trans(leg1); leg2 = Trans(leg2);
    int leg1margin, leg2margin, shipMargin;
        if((leg1margin = (leg1.y() - _terr->ElevAtX(leg1.x()))) <= 0)
        {
            if(Vel().Mag() > _legMaxImpactVel)
            {
               Stop();
               if(_state == 0) SpawnPartCloud(40, 10, 10, 2);
               _state = 3;
            }
            else if((Vel().Mag() > 5) || ((Acc() + Grav()).y > 0))
            {
                if(!_leg1coll) { Stop(); _leg1coll = true; }
                if((Tilt() < 45) && (Tilt() > -45))
                    SetAcc(pos2d(-5*qCos(Pos().AngRad()), 0.2*qSin(Pos().AngRad()), -0.3));
                else
                    SetAcc(pos2d(5*qCos(Pos().AngRad()), 0.2*qSin(Pos().AngRad()), 0.3));
            }
            else _leg1coll = false;
        }
        if((leg2margin = (leg2.y() - _terr->ElevAtX(leg2.x()))) <= 0)
        {
            if(Vel().Mag() > _legMaxImpactVel)
            {
               Stop();
               if(_state == 0) SpawnPartCloud(40, 10, 10, 2);
               _state = 2;
            }
            else if((Vel().Mag() > 5) || ((Acc() + Grav()).y > 0))
            {
                if(!_leg2coll) { Stop(); _leg2coll = true; }
                if((Tilt() < 45) && (Tilt() > -45))
                SetAcc(pos2d(5*qCos(Pos().AngRad()), 0.2*qSin(Pos().AngRad()), 0.3));
                else
                    SetAcc(pos2d(-5*qCos(Pos().AngRad()), 0.2*qSin(Pos().AngRad()), -0.3));

            }
            else _leg2coll = false;
        }
        if(_leg1coll && _leg2coll) Stop();
        if((shipMargin = (shipCenter.y() - _terr->ElevAtX(shipCenter.x())) - _fragileRadius) <= 0)
        {
            Stop();
            if(_state == 0) SpawnPartCloud(100, 20, 10, 10);
            _state = 1;
        }
        if(_state == 1)
        {
            Particle *newParticle = new Particle(Pos(),
                                                 pos2d(rand()%8-4, 0, 0),
                                                 Grav()*-3, 6, 5);
            _spawnedParticles.push_back(newParticle);
        }

        QPoint exhPort(Pos().Point());
        double exhPortFacing(Pos().AngRad());
        exhPort.rx() = exhPort.x() + (-25 * qSin(exhPortFacing));
        exhPort.ry() = exhPort.y() + (25 * qCos(exhPortFacing));

        if(_currThrustPerc>0)
        for(int i=0; i<12; i++)
            {
                Particle *newParticle = new Particle(pos2d(exhPort.x(), exhPort.y(), 0),
                                                     Vel() + (Acc()*-3) + ((Acc().Mag()>1)?(Acc()*(1/Acc().Mag())*-3):pos2d(0,0,0)) + pos2d(rand()%20-10, rand()%20-10, 0),
                                                     Grav()*3, 3, 3);
                newParticle->SetTerrain(_terr);
                _spawnedParticles.push_back(newParticle);
            }

            for(unsigned int i=0; i<_spawnedParticles.size(); i++)
                _spawnedParticles[i]->Step(dt);

    if(_state == 0)
        PhysicsObj::Step(dt);
}

void Ship::SetRandPos()
{
    int x = rand()%400 + 300;
    int y = rand()%100 + 400;
    x = Trans(QPoint(x, y)).x();
    y = Trans(QPoint(x, y)).y();
    double ang = qDegreesToRadians((double) (rand()%90 - 45));
    SetPos(pos2d(x, y, ang));
}

void Ship::SpawnPartCloud(int partNum, double partSize, double velMult, double gravMult, QPoint offset)
{
    for(int i=0; i<partNum; i++)
    {
        Particle *newPart = new Particle(
                    Pos() + pos2d(offset.x(), offset.y(), 0),
                    pos2d((((rand()%20) - 10) * velMult), (((rand()%20) - 10) * velMult), 0),
                    Grav()*gravMult,
                    10, partSize);
        _spawnedParticles.push_back(newPart);
    }
}

std::vector<double>* Ship::ParseData()
{
    std::vector<double> *infoVec = new std::vector<double>;
    infoVec->push_back(_currThrustPerc);
    infoVec->push_back(_currTorqPerc);
    infoVec->push_back(Tilt());
    infoVec->push_back(Vel().Mag());
    infoVec->push_back(_legMaxImpactVel);
    infoVec->push_back(_currFuel/_maxFuelCap);
    return infoVec;
}

void Ship::Reset()
{
    _currFuel = _maxFuelCap;
    _leg1coll = _leg2coll = false;
    _state = 10;
    while(_spawnedParticles.size() > 0)
    {
        delete _spawnedParticles[0];
        _spawnedParticles.erase(_spawnedParticles.begin());
    }
}

void GameWindow::StepScene()
{
    _lander->Step();
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
    int min = _verts[0].y();
    for(int i=1; i<_verts.size(); i++)
    {
        if(_verts[i].y() < min) min = _verts[i].y();
    }
    for(int i=0; i<_verts.size(); i++)
    {
        _verts[i].ry() = _verts[i].y() - (min*1.5);
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
    if(xPos < 0) xPos = 1;
    if(xPos > 1000) xPos = 999l;
    int i = 1;
    while((_verts[i].x() < xPos) && (i < (_verts.size()-1))) i++;
    double dY = (_verts[i].y() - _verts[i-1].y())/(double) (_verts[i].x() - _verts[i-1].x());
    return (xPos - _verts[i-1].x())*dY + _verts[i-1].y();
}

double Terrain::TiltAtX(int xPos)
{
    if(xPos < 0) xPos = 1;
    if(xPos > 1000) xPos = 999;
    int i = 1;
    while((_verts[i].x() < xPos) && (i < (_verts.size()-1))) i++;
    double dY = _verts[i].y() - _verts[i-1].y();
    double dX = _verts[i].x() - _verts[i-1].x();
    return qAtan2(dY,dX);
}

QPoint Trans(QPoint p)
{
    return QPoint(p.x(), W_HEIGHT-p.y());
}


void GameWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QPen line(Qt::black, 3, Qt::SolidLine);

    std::vector<double> *shipInfo = _lander->ParseData();

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(line);

    static QPixmap bg(":/img/my_bg");
    painter.drawPixmap(QPoint(0,0), bg);

    static QPixmap hud_bg(":/img/hud_static");
    painter.drawPixmap(QPoint(0, 0), hud_bg);

    static QPixmap torq_meter(":/img/hud_torq_meter");
    painter.translate(948, 80);
    painter.rotate((*shipInfo)[1]*90);
    painter.drawPixmap(QPoint(-1*torq_meter.width()/2, -1*torq_meter.height()/2), torq_meter);
    painter.resetTransform();

    static QPixmap thrust_meter(":/img/hud_thrust_meter");
    painter.translate(870, 176);
    for(int i=0; i<(*shipInfo)[0]*133; i++)
    {
        painter.drawPixmap(QPoint(0, 0), thrust_meter);
        painter.translate(0, -1);
    }
    painter.resetTransform();

    static QPixmap led_red(":/img/led_red"), led_orange(":/img/led_orange"), led_green(":/img/led_green");
    static QPoint vel_led(974, 273), ang_led(974, 243);
    if(qAbs((*shipInfo)[2]) > 45) painter.drawPixmap(ang_led, led_red);
    else if(qAbs((*shipInfo)[2]) > 25) painter.drawPixmap(ang_led, led_orange);
    else painter.drawPixmap(ang_led, led_green);

    if(qAbs((*shipInfo)[3]) > ((*shipInfo)[4])) painter.drawPixmap(vel_led, led_red);
    else if(qAbs((*shipInfo)[3]) > ((*shipInfo)[4]*0.66)) painter.drawPixmap(vel_led, led_orange);
    else painter.drawPixmap(vel_led, led_green);

    static QPixmap fuel_meter(":/img/hud_fuel_meter");
    QPoint zero_fuel(905, 128), fuel_len(81, 0);
    painter.drawPixmap(zero_fuel+(fuel_len*(*shipInfo)[5]), fuel_meter);

    _terr->Draw(&painter);
    _lander->Draw(&painter);

    StepScene();
    shipInfo->clear();
    delete shipInfo;
}

const QVector<QPoint> Terrain::Vec(){ return _verts; }

void GameWindow::on_pushButton_clicked()
{
    _lander->SetRandPos();
    _lander->Stop();
    _lander->Reset();
}

Particle::Particle(pos2d initPos, pos2d initVel, pos2d initGrav, double lifespanSec, double size):
    PhysicsObj(initPos, initVel, initGrav),
    _lifespanSec(lifespanSec),
    _size(size),
    _initSize(size),
    _collTerrain(NULL)
{
    if(_size < 2) _size = 2;
    _lifeTimer.start();
}

void Particle::SetTerrain(Terrain *terr)
{
    _collTerrain = terr;
}

void Particle::Draw(QPainter *painter)
{
    painter->translate(Pos().Point());
    painter->scale(_size, _size);
    painter->save();
    int ltime = _lifeTimer.elapsed();
    //painter->res
    QRadialGradient grad(QPoint(0,0), 1, QPoint(0,0));
    grad.setColorAt(0, QColor::fromRgb(255, 255*((double) ltime/(_lifespanSec*1000)), 0, 255-255*((double) ltime/(_lifespanSec*1000))));
    grad.setColorAt(1, QColor::fromRgb(255, 255*((double) ltime/(_lifespanSec*1000)), 0, 0));
    QPen part(QColor::fromRgb(255, 255*((double) ltime/(_lifespanSec*1000)), 0, 0), Qt::SolidPattern);
    QBrush part_brush(QColor::fromRgb(255, 255*((double) ltime/(_lifespanSec*1000)), 0, 255-255*((double) ltime/(_lifespanSec*1000))), Qt::SolidPattern);
    painter->setBrush(QBrush(grad));
    painter->setPen(part);
    painter->drawEllipse(QPoint(0,0), 1, 1);
    painter->restore();
    painter->resetTransform();
}

void Particle::Step(double dt)
{
    _size = (_initSize) * (1 + 5*(double) ((double) _lifeTimer.elapsed()/(_lifespanSec*1000)));
    if(_collTerrain != NULL)
    {
        QPoint pos = Trans(Pos().Point());
        if(pos.y() <= _collTerrain->ElevAtX(pos.x()))
        {
            double terrAng = qRadiansToDegrees(_collTerrain->TiltAtX(pos.x()));//+90;
            double velAng = qRadiansToDegrees(qAtan2((Vel().y*-1),Vel().x));
            double angDiff = velAng - terrAng;
            static bool fs = true;
            if(angDiff < -10 && angDiff > -170)
            {
            if(angDiff < -90) angDiff += 180;
                    RotateVel(angDiff);
                if(fs){fs = false; }
            }
        }
    }
    PhysicsObj::Step(dt);
}

bool Particle::Expired()
{
    if(_lifeTimer.elapsed() > (_lifespanSec*1000)) return true;
    else return false;
}


pos2d operator+(const pos2d& p1, const pos2d& p2)
{
    return pos2d(p1.x+p2.x, p1.y+p2.y, p1.ang+p2.ang);
}
