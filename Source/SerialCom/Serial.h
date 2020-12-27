#pragma once
#include <string>

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
        Serial();
        ~Serial();

       bool Connect(const std::string portname, const unsigned long  baudrate, const ByteSize &ByteSize,
                    const ConnectionMethod &method, const ParityCheck &parity, const StopBits &stopbits);
       bool Connect();
       void DisConnect();
       void Read();
       void Write();



    private:
        Internal::serialdata *data;
        SerialConnection::ByteSize bytesize;
        SerialConnection::ConnectionMethod connectionmethod;
        SerialConnection::ParityCheck prcheck;
        SerialConnection::StopBits stopbits;
        bool connected = false;
        std::string portname;
        std::string fullportpath;
    };
}