#pragma once
#include <string>
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
    public :
        Serial() {

        }
        virtual ~Serial() {


        }
        virtual void Connect(const std::string portname, int &baudrate, const ByteSize &ByteSize
            , const ConnectionMethod &method, const ParityCheck &parity,const StopBits &stopbits) = 0;



    };
}