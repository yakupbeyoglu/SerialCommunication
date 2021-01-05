#include <iostream>
#include "SerialCom/Serial.h"
#include "SerialCom/Host.h"
#include <thread>

int main(){
	std::cout<<"hello cmake test"<<std::endl;

  Host host;
 

  host.QueueManager("G0 Y10\n");
  host.QueueManager("G0 X10\n");
  host.QueueManager("M106 S255\n");
  host.QueueManager("M105\n");
  host.QueueManager("G0 Z10\n");

  host.QueueManager("G0 Y10\n");
  host.QueueManager("G0 X10\n");
  host.QueueManager("M106 S0\n");
  host.QueueManager("G0 Z0\n");

  host.QueueManager("G0 X180\n");
  host.QueueManager("M106 S255\n");
  host.QueueManager("G0 Z10\n");
  host.QueueManager("G0 Y0\n");
  host.QueueManager("G0 X50\n");




  host.Stop();

 
	return 0;
}
