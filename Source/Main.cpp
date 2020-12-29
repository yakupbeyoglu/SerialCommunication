#include <iostream>
#include <windows.h>
#include "SerialCom/Serial.h"

int main(){
	std::cout<<"hello cmake test"<<std::endl;
  SerialConnection::Serial serial;
  /*const std::string portname, const int &baudrate, const ByteSize &bytesize,
        const ConnectionMethod &method, const ParityCheck &parity, const StopBits &stopbits*/
  serial.Connect(3, 115200);
  serial.Read();

  serial.Write("M105 \n");
  serial.Read();
  serial.Write("M106 S255 \n");
  serial.Read();

  serial.Write("G0 X50 \n");
  serial.Read();

  serial.Write("M300 \n");
 
  /*
  SetCommState(serialHandle, &serialParams);
  DWORD writtenbytes = 0;
  // Set timeouts
  COMMTIMEOUTS timeout = { 0 };
  timeout.ReadIntervalTimeout = 50;
  timeout.ReadTotalTimeoutConstant = 50;
  timeout.ReadTotalTimeoutMultiplier = 50;
  timeout.WriteTotalTimeoutConstant = 50;
  timeout.WriteTotalTimeoutMultiplier = 10;

  SetCommTimeouts(serialHandle, &timeout);
  std::string txt = "M106 S255";
  auto s = 
  WriteFile(serialHandle, txt.c_str(), sizeof(txt.c_str()), &writtenbytes, NULL);

	*/
	return 0;
}