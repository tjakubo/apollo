#include "hardware.h"


Hardware::Hardware(QWidget *parent): QWidget(parent)
{
    rawDataSent = false;
}

void Hardware::setRawDataStatus(bool isSent)
{
    rawDataSent = isSent;
}

void Hardware::Measure()
{
    static boost::asio::io_service ios;
    static boost::asio::serial_port sp(ios, "/dev/ttyS0");
    sp.set_option(boost::asio::serial_port::baud_rate(9600));
    //meas measure;
    char tmp[16];
    //char meas[32];
    char meas_raw[32];
    bool begin, end;
    int iter;
    //std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    std::string message = "m";
    sp.write_some(boost::asio::buffer(message));
    //sp.read_some(boost::asio::buffer(tmp));
    begin = end = false;
    iter = 0;
    do
    {
        int read = sp.read_some(boost::asio::buffer(tmp));
        //std::cout << "Read " << read << " : " << tmp << std::endl;
        for(int i=0; (i<read) && !end; i++)
        {
            //std::cout << "read " << tmp[i] << std::endl;
            if(tmp[i]=='b'){ begin = true; iter = 0; }
            if(begin) meas_raw[iter] = tmp[i];
            if((tmp[i] == 'e') && begin){ end = true; meas_raw[iter + 1] = '\0'; }
            iter++;
        }

        //sp.close();
    } while(!end);
    //std::this_thread::sleep_for(std::chrono::milliseconds(50));
    //std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    //auto dt = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    //std::cout << meas << " and " << dt << "us elapsed\n
    meas meas_struct;
    QString str = (const char*) meas_raw;
    meas_struct.x = str.split(" ")[1].toInt();
    meas_struct.y = str.split(" ")[2].toInt();
    meas_struct.z = str.split(" ")[3].toInt();
    //qDebug() << "msrd" << meas_struct.x << " " << meas_struct.y << " " << meas_struct.z;
    emit sendMeasurement(meas_struct);
    if(rawDataSent) emit sendRawData(str);
}

/*Meas::Meas()
{
 x = y = z = 0;
 x_off = y_off = z_off = 0;
}

const int x() { return x + x_off; }
const int y() { return y + y_off; }
const int z() { return z + z_off; }
void setMeas(int xNew, int yNew, int zNew) { x = xNew; y = yNew; z = zNew; }
void setOffset(int xOff, int yOff, int zOff) { x_off = xOff; y_off = yOff; z_off = zOff; }
void resetOffset() { x_off = y_off = z_off = 0; }
int xAngle() { return*/

meas::meas(): x(0), y(0), z(0) {}
meas::meas(int nx, int ny, int nz): x(nx), y(ny), z(nz) {}
meas operator+ (meas m1, meas m2){ return meas(m1.x + m2.x, m1.y + m2.y, m1.z + m2.z); }
