#include <iostream>
#include "SerialCom/Serial.h"
#include "SerialCom/Host.h"
#include <thread>

int main(){
	std::cout<<"hello cmake test"<<std::endl;

  Host host;
  auto writethread = host.writing();

  auto readthread = host.reading();



  host.QueueManager("G0 X20\n");
  host.QueueManager("M106 S255\n");
  host.QueueManager("G0 Z10\n");

  host.QueueManager("G0 Y10\n");
  host.QueueManager("G0 X10\n");
  host.QueueManager("M106 S0\n");
  host.QueueManager("G0 Z0\n");

  host.QueueManager("G0 X180\n");
  host.QueueManager("M106 S255\n");
  host.QueueManager("G0 Z10\n");
  host.QueueManager("G0 Y0\n");




  //host.Write("M106 S255\n");
  //host.Write("M105\n");
  //host.Write("G0 X50\n");
  //readthread.join();



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
  host.Stop();

  writethread.join();
  readthread.join();
	return 0;
}
