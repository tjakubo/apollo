#include "hardware.hh"

/*!
 * Otwiera port MEAS_PORT, ustawia timeout odczytu TIMEOUT_MS,
 * baudrate 115200, domyślne dane kalibracji.
 */
Hardware::Hardware(QWidget *parent):
    QWidget(parent),
    _sp(_ios, MEAS_PORT),
    _reader(_sp, TIMEOUT_MS),
    _measCal(0, 0, 0, 1024)
{
    _sp.set_option(boost::asio::serial_port::baud_rate(115200));
    _rawDataSent = false;
    _avgSampleNum = 6;
}

/*!
 * "Pobierany" pomiar pochodzi ze stałej dosłownej SIM_MEAS_STR.
 * Trzeba zadbać żeby dane w stałej były w dobrym formacie, ale nie trzeba
 * liczyć i wstawiać do nich sumy kontrolnej.
 */
void Hardware::SimMeasure()
{
    meas meas_struct;
    QString str = SIM_MEAS_STR;
    meas_struct.x = str.split(" ")[1].toInt();
    meas_struct.y = str.split(" ")[2].toInt();
    meas_struct.z = str.split(" ")[3].toInt();
    meas_struct.p = str.split(" ")[4].toInt();

    emit NewMeasurement(Process(meas_struct));
    if(_rawDataSent) { emit NewRawData(str); }

}

/*!
 * Wysyła do sprzętu poprzez port znak 'm' co oznacza żądanie nowego pomiaru.
 * Potem odbiera dane (znaki) dopóki nie przypasuje ich do formatki pomiaru lub
 * czas pojedynczego odczytu nie zajmie dłużej niż TIMEOUT_MS.
 * Po dobrym odczycie sprawdza sumę kontrolną i jeśli wszystko jest OK, emituje sygnały
 * z PRZETWORZONYM nowym pomiarem (metodą Process() ).
 *
 * Format danych odczytu:
 *
 * b [akcel_x] [akcel_y] [akcel_z] [nap_pot] [suma_kont] e
 *
 * akcel_x/y/z : przyspieszenie w osi na 8 bitach (-128 : 128)
 * nap_pot : napiecie na potencjometrze w 10 bitach (0 : 1023
 * suma_kont : suma kontrolna, XOR po każdej danej pomiarowej w kolejności z formatu
 *
 * Np. "b -17 -40 51 401 405 e"
 */
void Hardware::Measure()
{
    bool begin, end;

    // wyslij 'm' do sprzetu - zadanie pomiaru
    std::string message = "m";
    _sp.write_some(boost::asio::buffer(message));

    begin = end = false;
    std::string out;
    char c; bool fail = false;

    // dopoki nie zapali sie flaga zakonczenia odczytu
    while(!end)
    {
        if( (fail = (!_reader.read_char(c))) ) break; // jesli timeout (fail = true) to koniec
        if(!begin && c=='b')
            begin = true;
        if(begin) out += c; // dodawaj pobrany znak az do znaku konca pomiaru
        if(begin && c=='e')
            end = true;
    }

    if(fail) // jesli skonczylo sie timeoutem, wywal blad i zakoncz
    {
        static unsigned int timeoutCount = 0;
        ++timeoutCount;
        qDebug() << "Brak odpowiedzi sprzetu (" << timeoutCount << ")";
        return;
    }

    // jesli OK, rozbij string pomiaru na dane
    meas meas_struct;
    QString str = out.c_str();
    meas_struct.x = str.split(" ")[1].toInt();
    meas_struct.y = str.split(" ")[2].toInt();
    meas_struct.z = str.split(" ")[3].toInt();
    meas_struct.p = str.split(" ")[4].toInt();
    int checksum = str.split(" ")[5].toInt();

    // jesli zla suma kontrolna, wywal blad i zakoncz
    if(checksum != (meas_struct.x ^ meas_struct.y ^ meas_struct.z ^ meas_struct.p))
    {
        static unsigned int  badReadCount = 0;
        ++badReadCount;
        qDebug() << "Nieprawidlowa suma kontrolna (" << badReadCount << ")";
        return;
    }

    // sygnaly zakonczenia pomiaru
    emit NewMeasurement(Process(meas_struct));
    if(_rawDataSent) { emit NewRawData(str); }
}

/*!
 * Robi:
 *
 * - uśrednianie wokół zadanej ilości ostatnich pomiarów (dla których było to wywyołane)
 *
 * - nakładanie zadanego offsetu na pomiar przyspieszen
 *
 * - zadane ograniczenie pomiaru potencjometru
 */
meas Hardware::Process(meas measurement)
{
    if(_avgSampleNum > 1)
    {
        // dopasuj wielkosc bufora probek, usuwaj/dodawaj az bedzie zadana
        static std::vector<meas> measHist;
        while((unsigned) _avgSampleNum < measHist.size()) measHist.erase(measHist.begin());
        while((unsigned) _avgSampleNum > measHist.size()) measHist.push_back(measurement);

        // zsumuj probki, podziel
        measHist.erase(measHist.begin());
        measHist.push_back(measurement);
        for(unsigned int i=0; i<(measHist.size()-1); i++) measurement = measurement + measHist[i];
        measurement = measurement/((int) measHist.size());
    }

    // naloz offset i max, wyslij
    measurement.OffsetXYZ(_measCal);
    if(measurement.p > _measCal.p) measurement.p = _measCal.p;
    return measurement;
}

void Hardware::SetCal(meas newCal)
{
    _measCal = newCal;
    emit NewCalibrationData(_measCal, _avgSampleNum);
}
meas Hardware::GetCal() { return _measCal; }

void Hardware::SetSampleNum(int newSampleNum)
{
    if(newSampleNum > SAMPLE_NUM_MAX) _avgSampleNum = SAMPLE_NUM_MAX;
    else if(newSampleNum < 1) _avgSampleNum = 1;
    else _avgSampleNum = newSampleNum;
    emit NewCalibrationData(_measCal, _avgSampleNum);
}
int Hardware::GetSampleNum(){ return _avgSampleNum; }

void Hardware::SetRawDataStatus(bool rawSent){ _rawDataSent = rawSent; }

meas::meas(): x(0), y(0), z(0) {}
meas::meas(int nx, int ny, int nz, int np): x(nx), y(ny), z(nz), p(np) {}
meas operator+ (meas m1, meas m2){ return meas(m1.x + m2.x, m1.y + m2.y, m1.z + m2.z, m1.p + m2.p); }
meas operator/ (meas m, int div){ return meas(m.x/div, m.y/div, m.z/div, m.p/div); }
void meas::OffsetXYZ(meas offset)
{
    this->x = this->x + offset.x;
    this->y = this->y + offset.y;
    this->z = this->z + offset.z;
}
