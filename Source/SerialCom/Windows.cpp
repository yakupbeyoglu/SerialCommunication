#include "Serial.h"
#include <Windows.h>
#include <iostream>
namespace Internal {
    struct serialdata {
        HANDLE connection;
        DCB serialParams = { 0 };
        COMMTIMEOUTS timeouts = { 0 };
        OVERLAPPED read, write;

    };
}

namespace SerialConnection {
    /*void Connect(const std::string portname, int &baudrate, const ByteSize &ByteSize,
                    const ConnectionMethod &method, const ParityCheck &parity, const StopBits &stopbits);
       void Connect();
       void DisConnect();
       void Read();
       void Write();

*/

    Serial::Serial() {
        data = new Internal::serialdata;

        memset(&data->read, 0, sizeof(OVERLAPPED));
        memset(&data->write, 0, sizeof(OVERLAPPED));
        data->serialParams.DCBlength = sizeof(DCB);

    }
    bool Serial::Connect(const std::string portname, const unsigned long  baudrate, const ByteSize &bytesize,
        const ConnectionMethod &method, const ParityCheck &parity, const StopBits &stopbits) {
 
      //  data->serialParams.fBinary = true;
        data->serialParams.BaudRate = baudrate;
        data->serialParams.ByteSize = int(bytesize);
        data->serialParams.StopBits = (stopbits == StopBits::OneStopBit) ?ONESTOPBIT : TWOSTOPBITS;

        data->serialParams.Parity = (parity == ParityCheck::Even) ? EVENPARITY :
            (parity == ParityCheck::Odd) ? ODDPARITY : NOPARITY;

      auto  connectionmethod = (method == ConnectionMethod::R) ? GENERIC_READ :
            (method == ConnectionMethod::RW) ?
            GENERIC_WRITE : GENERIC_READ | GENERIC_WRITE;
      fullportpath = portname;
     data->connection = CreateFile(fullportpath.c_str(), connectionmethod, 0, NULL, OPEN_EXISTING, 
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
     auto status = GetCommState(data->connection, &data->serialParams);

     if (data->connection == INVALID_HANDLE_VALUE) {
         std::cout << "Can not connect your slot";
         return false;
     }
     data->write.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
     data->timeouts.ReadIntervalTimeout = 50; // in milliseconds
     data->timeouts.ReadTotalTimeoutConstant = 50; // in milliseconds
     data->timeouts.ReadTotalTimeoutMultiplier = 50; // in milliseconds
     data->timeouts.WriteTotalTimeoutConstant = 0; // in milliseconds
     data->timeouts.WriteTotalTimeoutMultiplier = 5000; // in milliseconds
     SetCommTimeouts(data->connection, &data->timeouts);
 
     std::cout << "Successfully connected";
    status = SetCommState(data->connection, &data->serialParams);
    

     const char *test = "N10 G0 X20*22";

     unsigned long written = 0;
     status = WriteFile(data->connection, &test, sizeof(test), &written, NULL);

    }

    Serial::~Serial() {
        CloseHandle(data->connection);
        delete data;
    }


}