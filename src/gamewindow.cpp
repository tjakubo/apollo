#include "gamewindow.hh"
#include "ui_gamewindow.h"




// // //
// GameWindow

/*!
 * \param HWlink Wskaźnik na obiekt klasy sprzętu od którego ma brać pomiary dla sterowania
 */
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
    delete _terr;
    delete _lander;
    delete ui;
}

/*!
* Dane dla HUD pobierane są od lądownika w postaci wektora liczb o ustalonej strukturze.
* Wywołuje krok sceny.
*/
void GameWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QPen line(Qt::black, 3, Qt::SolidLine);

    // wez dane o statku
    std::vector<double> *shipInfo = _lander->ParseData();
    double thrustPerc = (*shipInfo)[0]; // procent silnika
    double torqPerc = (*shipInfo)[1];   // procent momentu
    double tiltDeg = (*shipInfo)[2];    // nachylenie w stopniach
    double velMag = (*shipInfo)[3];     // w.bezwzgl. predkosci
    double maxCollVel = (*shipInfo)[4]; // max predkosc uderzenia nog
    double fuelPerc = (*shipInfo)[5];   // procent paliwa
    shipInfo->clear();
    delete shipInfo;

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(line);

    // TLO
    static QPixmap bg(":/img/my_bg");
    painter.drawPixmap(QPoint(0,0), bg);

    // STALA CZESC HUD
    static QPixmap hud_bg(":/img/hud_static");
    painter.drawPixmap(QPoint(0, 0), hud_bg);

    // WSKAZNIK MOMENTU
    static QPixmap torq_meter(":/img/hud_torq_meter");
    painter.translate(948, 80);
    painter.rotate(torqPerc*90);
    painter.drawPixmap(QPoint(-1*torq_meter.width()/2, -1*torq_meter.height()/2), torq_meter);
    painter.resetTransform();

    // WSKAZNIK ODRZUTU
    static QPixmap thrust_meter(":/img/hud_thrust_meter");
    painter.translate(870, 176);
    for(int i=0; i<thrustPerc*133; i++)
    {
        painter.drawPixmap(QPoint(0, 0), thrust_meter);
        painter.translate(0, -1);
    }
    painter.resetTransform();

    static QPixmap led_red(":/img/led_red"), led_orange(":/img/led_orange"), led_green(":/img/led_green");
    static QPoint vel_led(974, 273), ang_led(974, 243);

    // LEDka NACHYLENIA
    if(qAbs(tiltDeg) > 45) painter.drawPixmap(ang_led, led_red);
    else if(qAbs(tiltDeg) > 25) painter.drawPixmap(ang_led, led_orange);
    else painter.drawPixmap(ang_led, led_green);

    // LEDka PREDKOSCI
    if(qAbs(velMag) > (maxCollVel)) painter.drawPixmap(vel_led, led_red);
    else if(qAbs(velMag) > (maxCollVel*0.66)) painter.drawPixmap(vel_led, led_orange);
    else painter.drawPixmap(vel_led, led_green);

    // MIERNIK PALIWA
    static QPixmap fuel_meter(":/img/hud_fuel_meter");
    QPoint zero_fuel(903, 122), fuel_len(83, 0);
    painter.drawPixmap(zero_fuel+(fuel_len*fuelPerc), fuel_meter);

    _terr->Draw(&painter);
    _lander->Draw(&painter);

    StepScene();
}

/*!
 * Generuje nowy teren oraz resetuje statek i nastawy sliderów (jak nowa gra).
 */
void GameWindow::showEvent(QShowEvent *event)
{
    on_resetButton_clicked();
    _terr->Generate(0, 1000, 4, 10, 0.15);
    QWidget::showEvent(event);
}

/*!
 * Jeśli ruszamy myszką trzymając lewy przycisk, statek jest przyciągany do kursora.
 * Wywoływany przy RUCHU myszką, więc trzymanie lewego przycisku w miejscu (bez ruchu) nad statkiem nic nie daje.
 */
void GameWindow::mouseMoveEvent(QMouseEvent* event) {
    int mX = event->pos().x();
    int mY = event->pos().y();

    // jesli mysza jest blisko, ustaw statek na jej pozycji
    double dist = qSqrt( pow((mX - _lander->Pos().x),2) + pow((mY - _lander->Pos().y),2));
    if(dist < 20) _lander->SetPos(pos2d(mX, mY, _lander->Pos().ang));
}

/*!
 * Wywołuje krok lądownika, ten wywołuje kroki swoich cząsteczek i mamy pełną fizykę sceny.
 */
void GameWindow::StepScene()
{
    _lander->Step();
}

/*!
 * Po otrzymaniu nowego pomiaru przykładamy nowe sterowanie do lądownika.
 * Tutaj można zmienić konwencję sterowania na inną, np. dołączyć sterowanie
 * potencjometrem.
 */
void GameWindow::NewMeasurementReceived(meas newMeas)
{
    int tilt = (((double) newMeas.y)/64) * 90; // WYCHYLENIE NA BOK
    if(newMeas.z < 0) tilt = 180 - tilt;       // czerwona dioda w swoja strone

    int set = (((double) newMeas.x)/64) * -90; // WYCHYLENIE W PRZOD
    if(newMeas.z < 0) set = 180 - set;         // w przeciwna niz czerwona dioda
    _lander->Steer(((double) set)/100, ((double) tilt)/100);
}

/*!
 * Reset lądownika i ustawień sceny do domyślnych wartości.
 * Nie re-generuje terenu.
 */
void GameWindow::on_resetButton_clicked()
{
    _lander->SetRandPos();
    _lander->Stop();
    _lander->Reset();
    _lander->SetPartDens(1);
    ui->partSlider->setSliderPosition(200);
    ui->gravSlider->setSliderPosition(200);
}

void GameWindow::on_partSlider_valueChanged(int value)
{
    _lander->SetPartDens(((double) value)/200);
}

void GameWindow::on_gravSlider_valueChanged(int value)
{
    int adjValue = value;
    if(value > 500) adjValue += (value-500);
    if(value > 800) adjValue += (value-800);
    _lander->SetGrav(pos2d(0, 1*( ((double) value)/200), 0));
}

pos2d operator+(const pos2d& p1, const pos2d& p2)
{
    return pos2d(p1.x+p2.x, p1.y+p2.y, p1.ang+p2.ang);
}
