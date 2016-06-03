#include "physics.hh"

PhysicsObj::PhysicsObj(): _pos(), _vel(), _acc(), _gravity(), _firstRun(true) {}

PhysicsObj::PhysicsObj(pos2d initPos, pos2d initVel, pos2d initGrav):
    _pos(initPos),
    _vel(initVel),
    _gravity(initGrav),
    _firstRun(true)
{}

/*!
* Działa jak prosta aproksymacja y(t+dt)=y(t)+(dy/dt)*dt, dt jest brane z argumentu lub timera,
* (dy/dt) to Vel dla Pos i Acc dla Vel. Siła (Acc) pozostaje niezmieniona.
*/
void PhysicsObj::Step(double dt)
{
    // jak 1 uruchomienie to dt=1ms
    double deltaTime = (double) _timer.restart()/1000;
    if(_firstRun){ deltaTime = 0.001; _firstRun = false;}
    if(dt > 0) deltaTime = dt; // podstaw argument jak sie da

    _pos = _pos + _vel*deltaTime;
    _vel = _vel + _acc*deltaTime + _gravity*deltaTime;

}

/*!
* Oprócz wyzerowania prędkości, restartuje ona także wewnętrzny timer aby zapobiec "nawijaniu"
* sie miniętego czasu kiedy chcemy w programie np. wybierać warunkowo między wywołaniem Step() a Stop().
*/
void PhysicsObj::Stop()
{
    _vel = pos2d(0, 0, 0);
    _timer.restart();
}


void PhysicsObj::AddForce(pos2d aForce)
{
    _acc = _acc + aForce;
}

/*!
* Współrzędne x,y pola _vel są traktowane jako zwykły wektor i obracane wokół początku układu
* współrzędnych.
* Pole kąta (_ang) nie ulega zmianie.
*/
void PhysicsObj::RotateVel(double angDeg)
{
    double angRad = qDegreesToRadians(angDeg);
    pos2d oldVel = _vel;
    _vel.x = oldVel.x*qCos(angRad) - oldVel.y*qSin(angRad);
    _vel.y = oldVel.x*qSin(angRad) + oldVel.y*qCos(angRad);
}
