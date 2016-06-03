#include "gameobjects.hh"

// // //
// Terrain

Terrain::Terrain(int xMin, int xMax, int vertCountMin, int vertCountMax, double dYmax)
{
    Generate(xMin, xMax, vertCountMin, vertCountMax, dYmax);
}

/*!
* Jeśli już coś istnieje, jest usuwane (można re-generować teren).
* \param xMin Od jakiej współrzędnej x ma się rozpoczynać teren
* \param xMax Do jakiej współrzędnej x ma być teren
* \param vertCountMin Minimalna liczba segmentów terenu
* \param vertCountMax Maksymalna liczba segmentów terenu
* \param dYmax Moduł maksymalnego nachylenia terenu (pochodnej prostej)
*/
void Terrain::Generate(int xMin, int xMax, int vertCountMin, int vertCountMax, double dYmax)
{
    if(!_verts.empty()) _verts.clear();

    int vertCount = (rand()%(vertCountMax - vertCountMin)) + vertCountMin;
    int xCurr = 0;
    int yCurr = rand()%( (int)((xMax - xMin)*dYmax)) - dYmax/2;
    _verts.push_back(QPoint(xCurr, yCurr));

    for(int i=0; i<vertCount; i++)
    {
        int dX = rand()%1000; // nowy punkt z zakresu 0-1000 !
        xCurr += dX;
        int yMax = dYmax * dX;
        int yCurr = rand()%(2*yMax) - yMax;
        _verts.push_back(QPoint(xCurr, yCurr));
    }

    // SKALOWANIE DO POZADANEGO ZAKRESU
    double normCoeff = (double) xMax/_verts[_verts.size()-1].x();
    for(unsigned int i=0; i<_verts.size(); i++)
    {
        _verts[i].rx() = _verts[i].x()*normCoeff;
        _verts[i].ry() = _verts[i].y()*normCoeff;
    }

    // minimalna wysokosc (moze byc ujemna)
    int min = _verts[0].y();
    for(unsigned int i=1; i<_verts.size(); i++)
    {
        if(_verts[i].y() < min) min = _verts[i].y();
    }
    for(unsigned int i=0; i<_verts.size(); i++)
    {
        _verts[i].ry() = _verts[i].y() - (min*1.5); // podneisz wszystko o 150% minimum
    }
}

const std::vector<QPoint> Terrain::Vec(){ return _verts; }

/*!
* Jeśli poda się zbyt duży lub mały argument, zwraca wysokość nad najbliższym poprawnym punktem
* (nie ekstrapoluje ostatniego odcinka)
*/
int Terrain::LowestElev()
{
    int lowest = 0;
    for(unsigned int i=0; i<_verts.size(); i++)
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
    if(xPos > 1000) xPos = 999;
    unsigned int i = 1;
    while((_verts[i].x() < xPos) && (i < (_verts.size()-1))) i++;
    double dY = (_verts[i].y() - _verts[i-1].y())/(double) (_verts[i].x() - _verts[i-1].x()); // nachyelenie odcinka
    return (xPos - _verts[i-1].x())*dY + _verts[i-1].y(); // y(x) = dx*dy + y(0)
}

/*!
* Korzysta z funkcji atan2(dy,dx)
* \return Nachylenie terenu w RADIANACH
*/
double Terrain::TiltAtX(int xPos)
{
    if(xPos < 0) xPos = 1;
    if(xPos > 1000) xPos = 999;
    unsigned int i = 1;
    while((_verts[i].x() < xPos) && (i < (_verts.size()-1))) i++;
    double dY = _verts[i].y() - _verts[i-1].y();
    double dX = _verts[i].x() - _verts[i-1].x();
    return qAtan2(dY,dX);
}

/*!
* Teren rysowany jest w postaci wypełnionego wieloboku z wierzchołkami w wierzchołkach terenu
* oraz jednym w prawym dolnym rogu okna, drugim w lewym dolnym rogu.
*/
void Terrain::Draw(QPainter *painter)
{
    QPolygon terrPoly; // wielobok do rysowania terenu

    painter->save();

    QBrush terrBrush(QColor::fromRgb(200, 200, 200));
    QPen terrPen(QColor::fromRgb(200, 200, 200));
    painter->setPen(terrPen);
    painter->setBrush(terrBrush);
    for(unsigned int i=0; i<_verts.size(); i++)
        terrPoly << Trans(_verts[i]); // normalnie teren ma postac przed przeksztalceniem do ukladu Qt!
    terrPoly << QPoint(1000, 650) << QPoint(0, 650);
    painter->drawPolygon(terrPoly);

    painter->restore();
    painter->resetTransform();
}

// Koniec Terrain
// // //

// // //
// Particle

Particle::Particle(pos2d initPos, pos2d initVel, pos2d initGrav, type pType, double lifespanSec, double size):
    PhysicsObj(initPos, initVel, initGrav),
    _partType(pType),
    _lifespanSec(lifespanSec),
    _size(size),
    _initSize(size),
    _collTerrain(NULL)
{
    if(_size < 2) _size = 2;
    _lifeTimer.start();
}

bool Particle::Expired()
{
    if(_lifeTimer.elapsed() > (_lifespanSec*1000)) return true;
    else return false;
}

/*!
* Cząsteczka jest rysowana zgodnie ze swoim polem typu (patrz metody DrawAsXXX() ).
*/
void Particle::Draw(QPainter *painter)
{
    painter->save();
    painter->translate(Pos().Point());
    switch(_partType)
    {
    case (type::Flame): // JESLI OGIEN
    {
        DrawAsFlame(painter);
        break;
    }
    case (type::Spark): // JESLI ISKRA
    {
        DrawAsSpark(painter);
        break;
    }
    case (type::Dust):  // JESLI PYL
    {
        DrawAsDust(painter);
        break;
    }
    }
    painter->restore();
    painter->resetTransform();
}

/*!
 * Cząsteczka jest rysowana jako plamka ROZSZERZAJĄCA się w czasie, zmieniająca w trakcie życia
 * kolor CZERWONY->ŻÓŁTY, rysowana jako KOŁO wypełnione gradientem PRZEZROCZYSTYM NA BRZEGU.
 */
void Particle::DrawAsFlame(QPainter *painter)
{
    painter->save();
    painter->scale(_size, _size);
    int ltime = _lifeTimer.elapsed();
    double lifePerc = (double) ltime/(_lifespanSec*1000);
    if(lifePerc > 1) lifePerc = 1;
    QRadialGradient grad(QPoint(0,0), 1, QPoint(0,0));

    // gradient w srodku
    // czerw->zolty od czasu, 0-100% przezroczystowsci od czasu
    grad.setColorAt(0, QColor::fromRgb(255, 255*(lifePerc), 0, 255-255*(lifePerc)));
    // gradient na brzegu
    // kolor czerw->zolty od czasu, 100% przezroczysty
    grad.setColorAt(1, QColor::fromRgb(255, 255*(lifePerc), 0, 0));
    // pen, przezroczysty, bez tego dziwne rzeczy sie dzieja
    QPen partF(QColor::fromRgb(255, 255*(lifePerc), 0, 0), Qt::SolidPattern);
    painter->setBrush(QBrush(grad));
    painter->setPen(partF);
    painter->drawEllipse(QPoint(0,0), 1, 1);
    painter->restore();
}

/*!
 * Cząsteczka jest rysowana jako KRÓTKA LINIA koloru ŻÓŁTEGO rysowana w KIERUNKU PRĘDKOŚCI.
 * Częściowo skalowana prędkoscią (z niskim progiem maksymalnym).
 */
void Particle::DrawAsSpark(QPainter *painter)
{
    painter->save();
    QPen partS(Qt::yellow);
    painter->setPen(partS);
    pos2d pVel(Vel());
    // skaluj dlugosc predkoscia, ale z ograniczeniem dlugosci
    if(pVel.Mag() > 5)
        pVel = pVel*(1/pVel.Mag())*5;
    painter->drawLine(QPoint(0, 0), pVel.Point());
    painter->restore();
}

/*!
 * Cząsteczka rysowana jako MINIMALNIE ZWIĘKSZAJĄCE SIĘ koło o kolorze SZARYM wypełnione gradientem
 * PRZEZROCZYSTYM NA BRZEGACH, stające się z czasem całkowicie PRZEZROCZYSTE.
 */
void Particle::DrawAsDust(QPainter *painter)
{
    painter->save();
    painter->scale(_size/2, _size/2);
    int ltime = _lifeTimer.elapsed();
    double lifePerc = (double) ltime/(_lifespanSec*1000);
    if(lifePerc > 1) lifePerc = 1;
    QRadialGradient grad(QPoint(0,0), 1, QPoint(0,0));

    // szary gradient w srodku
    grad.setColorAt(0, QColor::fromRgb(200, 200, 200, 255-255*(lifePerc)));
    // przezroczysty na brzegu
    grad.setColorAt(1, QColor::fromRgb(200, 200, 200, 0));
    // pen, przezroczysty, bez tego dziwne rzeczy sie dzieja
    QPen partF(QColor::fromRgb(200, 200, 200, 0), Qt::SolidPattern);
    painter->setBrush(QBrush(grad));
    painter->setPen(partF);
    painter->drawEllipse(QPoint(0,0), 1, 1);
    painter->restore();
}

/*!
* Metoda ta obsługuje zmianę wielkości cząsteczki od czasu życia oraz kolizje z
* terenem, o ile ten został ustawiony.
* Kolizja z terenem polega na zmianie prędkości na równoległą do terenu (bez strat w module).
* Kolizje pod kątem ostrzejszym nic 10 stopni są ignorowane.
* Potem wywoływany jest krok fizyki cząsteczki z zadanym odcinkiem czasu.
*
* \param dt Zadana długość kroku do obliczeń dla fizyki cząsteczki (tylko dla klasy bazowej)
*/
void Particle::Step(double dt)
{
    _size = (_initSize) * (1 + 5*(double) ((double) _lifeTimer.elapsed()/(_lifespanSec*1000)));
    if(_collTerrain != NULL)
    {
        QPoint pos = Trans(Pos().Point());
        if(pos.y() <= _collTerrain->ElevAtX(pos.x()))
        {
            double terrAng = qRadiansToDegrees(_collTerrain->TiltAtX(pos.x()));
            double velAng = qRadiansToDegrees(qAtan2((Vel().y*-1),Vel().x));
            double angDiff = velAng - terrAng;
            static bool fs = true;
            if(angDiff < -5 && angDiff > -185l)
            {
                if(angDiff < -110) angDiff +=180;
                else if((angDiff < -90) && (rand()%10 > 3)) angDiff +=180;
                else if((angDiff < -70) && (rand()%10 < 3)) angDiff +=180;
                RotateVel(angDiff);
                if(fs){fs = false; }
            }
        }
    }
    PhysicsObj::Step(dt);
}

void Particle::SetTerrain(Terrain *terr)
{
    _collTerrain = terr;
}

// Koniec Particle
// // //

// // //
// Ship


Ship::Ship(Terrain *terr):
    PhysicsObj(pos2d(0, 0, 0), pos2d(0, 0, 0), pos2d(0, 5, 0)),
    _terr(terr)
{
    _maxThrust = 20;
    _maxTorq = 1;
    _state = 0;

    _maxFuelCap = 1000;
    _maxFuelConsPerSec = 5;
    _currFuel = _maxFuelCap;

    _legAngle = 45;
    _legLength = 50;
    _legMaxImpactVel = 50;
    _fragileRadius = 30;
    _leg1coll = _leg2coll = false;
    SetRandPos();
}

Ship::~Ship()
{
    while(!_spawnedParticles.empty()) {
        delete _spawnedParticles[0];
        _spawnedParticles.erase(_spawnedParticles.begin());
    }
}

/*!
* Funkcja ta obsługuje większą część całej funkcjonalnosci programu:
* - obsługuje kolizje statku z terenem, poprzez nogi i bez nich
* - obsługuje imitację "spadania" lądownika na drugą nogę gdy stio na jednej
* - obsługuje utratę paliwa w czasie
* - tworzy cząsteczki imitujące gazy odrzutowe, wybuchy, ogień po zniszczeniu
* - wywołuje fizykę cząsteczek
* - niszczy czasteczki ktorym minal czas zycia
*
* \param dt Zadana długość kroku do obliczeń dla fizyki statku i cząsteczek
*/
void Ship::Step(double dt)
{
    //qDebug() << Tilt() << " : " << Pos().AngDeg();
    //qDebug() << _particleDensity;
    //qDebug() << Vel().AngRad();

    // konsumpcja paliwa
    _currFuel -= _maxFuelConsPerSec*_currThrustPerc;
    if(_currFuel < 0) _currFuel = 0;

    // kolizja nog z terenem
    double ang = Pos().AngRad();
    double legOffset = _legAngle*(2.0*3.1416)/360.0;
    QPoint leg1(_legLength*qCos(ang+legOffset), _legLength*qSin(ang+legOffset)),
            leg2(_legLength*qCos(ang+3*legOffset), _legLength*qSin(ang+3*legOffset));
    QPoint shipCenter = Trans(Pos().Point());
    leg1 += Pos().Point(); leg2 += Pos().Point();
    leg1 = Trans(leg1); leg2 = Trans(leg2);
    int leg1margin, leg2margin, shipMargin; // odleglosc nog i statku od terenu

    // jesli pierwsza noga dotyka ziemi
    if((leg1margin = (leg1.y() - _terr->ElevAtX(leg1.x()))) <= 0)
    {
        if(!_leg1coll) // jesli dopiero weszla w kolizje, stworz pod nia chmure pylu
        {
            double velMult = Vel().Mag()/3;
            if(velMult > 20) velMult = 20;
            QPoint tleg1 = Trans(leg1);
            for(int i=0; i<(5*_particleDensity*velMult); i++)
            {
                Particle *newParticle = new Particle(pos2d(tleg1.x(), tleg1.y(), 0), pos2d(rand()%30-15, -1*(rand()%10)+velMult*-2, 0),
                                                     Grav()*5, Particle::type::Dust, 6, 2);
                newParticle->SetTerrain(_terr);
                _spawnedParticles.push_back(newParticle);
            }
        }

        if(Vel().Mag() > _legMaxImpactVel) // jesli trzaslo za szybko
        {
            // stop, chmura iskier, zmiana stanu
            Stop();
            if(_state == 0) SpawnPartCloud(40*_particleDensity, 10, 10, 10, Particle::type::Spark);
            _state = 3;
        }
        else if((Vel().Mag() > 5) || ((Acc() + Grav()).y > 0)) // jesli statek leci szybko lub NIE przyspiesza w gore
        {
            if(!_leg1coll) { Stop(); _leg1coll = true; } // zatrzymaj, oznacz kolizje

            if((Tilt() < 45) && (Tilt() > -45))
                SetAcc(pos2d(-5*qCos(Pos().AngRad()), 0.2*qSin(Pos().AngRad()), -0.3)); // jesli male wychylenie, stawiaj do pionu
            else
                SetAcc(pos2d(5*qCos(Pos().AngRad()), 0.2*qSin(Pos().AngRad()), 0.3)); // jesli duze, wywracaj
        }
        else _leg1coll = false; // jesli statek leci wolno ORAZ przyspiesza w gore - pusc, odznacz kolizje
    }

    // jesli druga noga dotyka ziemi
    if((leg2margin = (leg2.y() - _terr->ElevAtX(leg2.x()))) <= 0)
    {
        if(!_leg2coll) // jesli dopiero weszla w kolizje, stworz pod nia chmure pylu
        {
            double velMult = Vel().Mag()/3;
            if(velMult > 20) velMult = 20;
            QPoint tleg2 = Trans(leg2);
            for(int i=0; i<(5*_particleDensity*velMult); i++)
            {
                Particle *newParticle = new Particle(pos2d(tleg2.x(), tleg2.y(), 0), pos2d(rand()%30-15, -1*(rand()%10)+velMult*-2, 0),
                                                     Grav()*5, Particle::type::Dust, 6, 2);
                newParticle->SetTerrain(_terr);
                _spawnedParticles.push_back(newParticle);
            }
        }

        if(Vel().Mag() > _legMaxImpactVel) // jesli trzaslo za szybko
        {
            // stop, chmura iskier, zmiana stanu
            Stop();
            if(_state == 0) SpawnPartCloud(40*_particleDensity, 10, 10, 10, Particle::type::Spark);
            _state = 2;
        }
        else if((Vel().Mag() > 5) || ((Acc() + Grav()).y > 0)) // jesli statek leci szybko lub NIE przyspiesza w gore
        {
            if(!_leg2coll) { Stop(); _leg2coll = true; } // zatrzymaj, oznacz kolizje
            if((Tilt() < 45) && (Tilt() > -45))
                SetAcc(pos2d(5*qCos(Pos().AngRad()), 0.2*qSin(Pos().AngRad()), 0.3)); // jesli male wychylenie, stawiaj do pionu
            else
                SetAcc(pos2d(-5*qCos(Pos().AngRad()), 0.2*qSin(Pos().AngRad()), -0.3)); // jesli duze, wywracaj

        }
        else _leg2coll = false; // jesli statek leci wolno ORAZ przyspiesza w gore - pusc, odznacz kolizje
    }

    // jesli obie nogi w kontakcie z ziemia, zatrzymaj
    if(_leg1coll && _leg2coll) Stop();

    // jesli srodek statku w zbyt bliskim kontakcie z podlozem
    if((shipMargin = (shipCenter.y() - _terr->ElevAtX(shipCenter.x())) - _fragileRadius) <= 0)
    {
        // stop, chmura iskier, status = zniszczony
        Stop();
        if(_state == 0) SpawnPartCloud(100*_particleDensity, 20, 10, 10, Particle::type::Spark);
        _state = 1;
    }

    // jesli lezy zepsuty
    if(_state == 1)
    {
        // tworz czasteczke palacego sie ognia
        Particle *newParticle = new Particle(Pos(),
                                             pos2d(rand()%8-4, 0, 0),
                                             Grav()*-3, Particle::type::Flame, 6, 5);
        _spawnedParticles.push_back(newParticle);

        // od czasu do czasu tez iskre
        if(rand()%20 > (18/(_particleDensity)))
        {
            Particle *newParticle = new Particle(pos2d(Pos().x + (rand()%30 - 15), Pos().y + (rand()%30 - 15), 0),
                                                 pos2d(rand()%200 - 100, rand()%100*-1, 0),
                                                 Grav()*10, Particle::type::Spark, 6, 5);
            _spawnedParticles.push_back(newParticle);
        }
    }

    // CZASTECZKI Z ODRZUTU
    QPoint exhPort(Pos().Point()); // punkcik z ktorego maja leciec
    double exhPortFacing(Pos().AngRad());
    exhPort.rx() = exhPort.x() + (-25 * qSin(exhPortFacing));
    exhPort.ry() = exhPort.y() + (25 * qCos(exhPortFacing));

    // jesli silnik wlaczony
    if(_currThrustPerc>0 && _currFuel>0)
        for(int i=0; i<(16*_particleDensity*_currThrustPerc); i++)
        {
            // stworz czasteczke
            // predkosc: [pred. statku] + stala*[przysp statku] + stala*[stala w kier przyspieszenia + [czesc losowa]
            Particle *newParticle = new Particle(pos2d(exhPort.x(), exhPort.y(), 0),
                                                 Vel() + (Acc()*-10) + ((Acc().Mag()>1)?(Acc()*(1/Acc().Mag())*-5):pos2d(0,0,0)) + pos2d(rand()%20-10, rand()%20-10, 0),
                                                 Grav()*3, Particle::type::Flame, 3, 3);
            newParticle->SetTerrain(_terr);
            _spawnedParticles.push_back(newParticle);
        }

    // wywolaj krok dla swoich czasteczek
    for(unsigned int i=0; i<_spawnedParticles.size(); i++)
        _spawnedParticles[i]->Step(dt);

    // usun te czasteczki ktore juz przezyly swoje
    for(unsigned int i=0; i<_spawnedParticles.size(); i++)
    {
        if(_spawnedParticles[i]->Expired())
        {
            delete _spawnedParticles[i];
            _spawnedParticles.erase(_spawnedParticles.begin() + i);
        }
    }

    // jesli nie zniszczony, krok
    if(_state == 0)
        PhysicsObj::Step(dt);
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
    // limitowanie sterownia predkosci obrotowej jesli wieksza niz 4 i w tym samym kierunku
    if((qAbs(Vel().AngRad()) > 4) && ((actTorque*Vel().AngRad()) > 0))
    {
        if(qAbs(Vel().AngRad()) > 5) actTorque = 0;
        else actTorque = actTorque * (5-qAbs(Vel().AngRad()));
    }


    SetAcc(pos2d(actThrust * qSin(Pos().AngRad()), -1*actThrust * qCos(Pos().AngRad()), actTorque));
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
            _spawnedParticles[i]->Draw(painter);
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

void Ship::SpawnPartCloud(int partNum, double partSize, double velMult, double gravMult, Particle::type pType, QPoint offset)
{
    for(int i=0; i<partNum; i++)
    {
        Particle *newPart = new Particle(
                    Pos() + pos2d(offset.x(), offset.y(), 0),
                    pos2d((((rand()%20) - 10) * velMult), (((rand()%20) - 10) * velMult), 0),
                    Grav()*gravMult, pType,
                    10, partSize);
        _spawnedParticles.push_back(newPart);
    }
}

std::vector<double>* Ship::ParseData()
{
    std::vector<double> *infoVec = new std::vector<double>;
    infoVec->push_back(_currThrustPerc);        // [0] : procent silnika
    infoVec->push_back(_currTorqPerc);          // [1] : procent momentu
    infoVec->push_back(Tilt());                 // [2] : nachylenie w stopniach
    infoVec->push_back(Vel().Mag());            // [3] : w.bezwzgl. predkosci
    infoVec->push_back(_legMaxImpactVel);       // [4] : max predkosc uderzenia nog
    infoVec->push_back(_currFuel/_maxFuelCap);  // [5] : procent paliwa
    return infoVec;
}

void Ship::Reset()
{
    _currFuel = _maxFuelCap;                // pelne paliwo
    _leg1coll = _leg2coll = false;          // brak kolizji
    _state = 10;                            // status : ok
    while(_spawnedParticles.size() > 0)     // niszcz czasteczki
    {
        delete _spawnedParticles[0];
        _spawnedParticles.erase(_spawnedParticles.begin());
    }
}

// Koniec Ship
// // //

/*!
 * Zamienia układ z początkiem w lewym dolnym rogu na układ z początkiem w lewym górnym rogu. Korzysta ze stałej W_HEIGHT.
 */
QPoint Trans(QPoint p)
{
    return QPoint(p.x(), W_HEIGHT-p.y());
}

