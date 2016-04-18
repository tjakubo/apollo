#include "hardware.h"


Hardware::Hardware(QWidget *parent): QWidget(parent) {}

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
}
