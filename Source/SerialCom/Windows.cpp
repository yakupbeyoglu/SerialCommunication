#include "Serial.h"
#include <Windows.h>
#include <iostream>

namespace Internal {
    struct serialdata {
        HANDLE connection;
        DCB parameters = { 0 };
        COMMTIMEOUTS timeouts = { 0 };
        OVERLAPPED read, write;

    };
}

namespace SerialConnection {


    Serial::Serial(const ByteSize &size, const ConnectionMethod &method,
        const ParityCheck&parity, const StopBits stopbit) :
        bytesize(size), connectionmethod(method), parity(parity), stopbits(stopbit) {

        // memory allocation of read , write handler events
        this->data = new Internal::serialdata;
        memset(&data->read, 0, sizeof(OVERLAPPED));
        memset(&data->write, 0, sizeof(OVERLAPPED));
        this->data->connection = nullptr;
        isconnect = false;
        data->parameters.DCBlength = sizeof(DCB);
    }

    Serial::~Serial() {
        if (isconnect)
            DisConnect();
        delete data;
    }

    bool Serial::Connect(const int &portnumber, const int &baudrate) {

        if (isconnect)
            return true;
        std::string port = "COM";
        port.append(std::to_string(portnumber));
        std::cout << "connected port = " << port;
        auto  connectmethod = (connectionmethod == ConnectionMethod::R) ? GENERIC_READ :
            (connectionmethod == ConnectionMethod::W) ?
            GENERIC_WRITE : GENERIC_READ | GENERIC_WRITE;

        data->connection = CreateFile(port.c_str(), connectmethod, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
   
        
        if (data->connection == nullptr) {
            std::cout << "Warning :" << port << " can not found!" << std::endl;
            return false;
        }
        // refresh each connect
        memset(&data->read, 0, sizeof(OVERLAPPED));
        memset(&data->write, 0, sizeof(OVERLAPPED));

        data->timeouts.ReadIntervalTimeout = 50; // in milliseconds
        data->timeouts.ReadTotalTimeoutConstant = 50; // in milliseconds
        data->timeouts.ReadTotalTimeoutMultiplier = 50; // in milliseconds
        data->timeouts.WriteTotalTimeoutConstant = 100; // in milliseconds
        data->timeouts.WriteTotalTimeoutMultiplier = 5000; // in milliseconds
        
        SetCommTimeouts(data->connection, &data->timeouts);
        
        // Event creation for readand write
        data->read.hEvent = CreateEvent(NULL, true, false, NULL);
        data->write.hEvent = CreateEvent(NULL, true, false, NULL);

        data->parameters.DCBlength = sizeof(DCB);
        data->parameters.BaudRate = baudrate;
        data->parameters.ByteSize = int(bytesize);
        data->parameters.StopBits = (stopbits == StopBits::OneStopBit) ? ONESTOPBIT : TWOSTOPBITS;
        data->parameters.Parity = (parity == ParityCheck::Even) ? EVENPARITY :
                                (parity == ParityCheck::Odd) ? ODDPARITY : NOPARITY;
        // in windows must be daya in binary
          data->parameters.fBinary = true;
          data->parameters.fOutxCtsFlow = false;
          data->parameters.fOutxDsrFlow = false;
          data->parameters.fDsrSensitivity = false;
          // rts and dtr pins enable to communicate
          data->parameters.fRtsControl = RTS_CONTROL_ENABLE;
          data->parameters.fDtrControl = DTR_CONTROL_ENABLE;

        if (!SetCommState(data->connection, &data->parameters)) {
            std::cout << "Warning :" << port << " connection not established!" << std::endl;

            if (data->read.hEvent != NULL)
                CloseHandle(data->read.hEvent);
            if (data->write.hEvent != NULL)
                CloseHandle(data->write.hEvent);

            return false;
        }

        isconnect = true;
        return isconnect;
    }


    bool Serial::DisConnect() {
        if (!isconnect || data->connection == NULL)
            return true;

        if (data->read.hEvent != NULL)
            CloseHandle(data->read.hEvent);
        
        if (data->write.hEvent != NULL)
            CloseHandle(data->write.hEvent);

        CloseHandle(data->connection);
        isconnect = false;
        return true;
     }

    int Serial::Write(const std::string &text) {

        if (!isconnect || data->connection == NULL)
            return 0;
        DWORD writtenbyte;
        bool status = WriteFile(data->connection, LPSTR(text.c_str()), text.length(), &writtenbyte, &data->write);
        // below handle sync. problem
        if (!status && GetLastError() == ERROR_IO_PENDING) {
            if (WaitForSingleObject(data->write.hEvent, 1000))
                writtenbyte = 0;
            else {
                GetOverlappedResult(data->connection, &data->write, &writtenbyte, false);
                data->write.Offset += writtenbyte;
            }
        }
        return int(writtenbyte);
    }

    int Serial::Read() {

        if (!isconnect)
            return  0;
        bool status;
        DWORD readenbyte, errors;
        COMSTAT stat;
        DWORD read;


        readenbyte = WaitForSingleObject(data->read.hEvent, 500);
        if (readenbyte == WAIT_TIMEOUT) {
            readenbyte = WaitForSingleObject(data->read.hEvent, 500);
        }
        ClearCommError(data->connection, &errors, &stat);
        //status = WaitCommEvent(data->connection, &errors, );

        // check the n.o byte reciverd from the serial provider , if 0 return!
        if (!stat.cbInQue)
            return 0;
        // how many byte can be readeable ?
        readenbyte = (DWORD)stat.cbInQue;
        char *z = new char[readenbyte];
      
       
       status = ReadFile(data->connection, z, readenbyte, &read, &data->read);
  
       std::cout << z << std::endl;
       std::string a = z;
       if(a.find("ok") != std::string::npos)
           return 10;
        if (!status) {
            // below can be delete
            if (GetLastError() == ERROR_IO_PENDING) {
                WaitForSingleObject(data->read.hEvent, 2000);
                return(int(readenbyte));
            }
            return(0);

        }

        return int(readenbyte);
    }
  


}