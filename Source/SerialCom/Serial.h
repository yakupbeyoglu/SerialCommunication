#pragma once
#include <string>
#include <vector>

namespace Internal {
    //opaque type method
    struct serialdata;
}
namespace SerialConnection {
    enum class ConnectionMethod {
        // read , write, read & write
        R = 0,
        W = 1,
        RW = 2

    };
    enum class ByteSize {
        five = 5,
        six = 6,
        seven = 7,
        eight = 8
    };

    enum class ParityCheck {
        NoParity = 0,
        Odd = 1,
        Even = 1

    };

    enum class StopBits {
        OneStopBit = 1,
        TwoStopBit = 2

    };

    class Serial {
    public:
        Serial(const ByteSize &size = ByteSize::eight,const ConnectionMethod &method = ConnectionMethod::RW,
            const ParityCheck&parity = ParityCheck::NoParity, const StopBits stopbit = StopBits::TwoStopBit);
        ~Serial();

       bool Connect(const int &portnumber,const int &baudrate);
       bool DisConnect();

       int Read();
       int Write(const std::string &text);
       bool IsConnect()const;
       std::vector<std::pair<std::string,std::string>> GetAvaliablePorts();
       void SetPort(const int &portnumber,const int baudrate) {
           if (isconnect) {
               DisConnect();
               Connect(portnumber, baudrate);
           }

       }

    private:
        Internal::serialdata *data;
        SerialConnection::ByteSize bytesize;
        SerialConnection::ConnectionMethod connectionmethod;
        SerialConnection::ParityCheck parity;
        SerialConnection::StopBits stopbits;
        bool isconnect = false;
        std::string portname;
        std::string fullportpath;
        // read , write  or RW
        ConnectionMethod connectmethod;
        std::vector<std::pair<std::string, std::string>> avaliableports{};
    };
}