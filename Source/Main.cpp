#include <iostream>
#include "SerialCom/Serial.h"

int main(){
	std::cout<<"hello cmake test"<<std::endl;
	SerialConnection::Serial serial;
	serial.Connect(3,115200);
	serial.Read();
	serial.Write("G0 X20 \n");
	serial.Read();
	serial.Write("M106 S255 \n");
	serial.Read();
	serial.Write("M105 \n");
	serial.Read();


 //SerialConnection::Serial serial;
  /*const std::string portname, const int &baudrate, const ByteSize &bytesize,
        const ConnectionMethod &method, const ParityCheck &parity, const StopBits &stopbits*/
/*  serial.Connect(3, 115200);

 auto z = serial.Read();
  while (z != 10) {
      z = serial.Read();

  }
  serial.Write("M105 \n");
   z = serial.Read();
  while (z != 10) {
      z = serial.Read();

  }
  serial.Write("M106 S255 \n");
  z = serial.Read();
  while (z != 10) {
      z = serial.Read();

  }

  serial.Write("G0 X50 \n");
  z = serial.Read();
  while (z != 10) {
      z = serial.Read();

  }

  serial.Write("M300 \n");
 */
  
	return 0;
}
