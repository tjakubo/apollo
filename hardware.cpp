#include "hardware.hh"


Hardware::Hardware(QWidget *parent):
    QWidget(parent),
    _sp(_ios, MEAS_PORT),
     _reader(_sp, 50),
    _measCal(0, 0, 0, 1024)
{
   // _ios.run();
    _sp.set_option(boost::asio::serial_port::baud_rate(9600));
    _rawDataSent = false;
    _avgSampleNum = 1;
   // _timer.setSingleShot(true);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(RePoll()));
}

void Hardware::Measure()
{
    //char tmp[16];
    //char meas_raw[32];
    bool begin, end;
    //int iter;
    //_timer.start(TIMEOUT_MS);
    std::string message = "m";
    _sp.write_some(boost::asio::buffer(message));
    begin = end = false;
    //iter = 0;
    /*do
    {
        int read = _sp.read_some(boost::asio::buffer(tmp));
        for(int i=0; (i<read) && !end; i++)
        {
            if(tmp[i]=='b'){ begin = true; iter = 0; }
            if(begin) meas_raw[iter] = tmp[i];
            if((tmp[i] == 'e') && begin){ end = true; meas_raw[iter + 1] = '\0'; }
            iter++;
        }

    } while(!end && _timer.isActive());*/

std::string out;
char c; bool fail = false;
while(!end)
{
    if( (fail = (!_reader.read_char(c))) ) break;
    if(!begin && c=='b')
        begin = true;
    if(begin) out += c;
    if(begin && c=='e')
        end = true;
}

    if(fail)
    {
        static unsigned int timeoutCount = 0;
        ++timeoutCount;
        qDebug() << "Brak odpowiedzi sprzetu (" << timeoutCount << ")";
        return;
    }

    meas meas_struct;
    //QString str = (const char*) meas_raw;
    QString str = out.c_str();
    meas_struct.x = str.split(" ")[1].toInt();
    meas_struct.y = str.split(" ")[2].toInt();
    meas_struct.z = str.split(" ")[3].toInt();
    meas_struct.p = str.split(" ")[4].toInt();

    emit NewMeasurement(Process(meas_struct));

    if(_rawDataSent) { emit NewRawData(str); }
}

meas Hardware::Process(meas measurement)
{
    if(_avgSampleNum > 1)
    {
     static std::vector<meas> measHist;
     while((unsigned) _avgSampleNum < measHist.size()) measHist.erase(measHist.begin());
     while((unsigned) _avgSampleNum > measHist.size()) measHist.push_back(measurement);

     measHist.erase(measHist.begin());
     measHist.push_back(measurement);
     for(unsigned int i=0; i<(measHist.size()-1); i++) measurement = measurement + measHist[i];
     measurement = measurement/((int) measHist.size());
    }

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
