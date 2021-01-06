#include <iostream>
#include "SerialCom/Serial.h"
#include "SerialCom/Host.h"
#include <thread>

std::vector<std::string> requesttest = { "G0 X20\n","G0 Y10\n","M106 S255\n","G0 Z10\n","M106 S0\n","G0 Y0\n" ,"G28\n", "M0\n" };
std::vector<std::string> requesttest2 = { "G0 X20\n","G0 Y10\n","M106 S255\n","G0 Z10\n","M106 S0\n","G0 Y0\n" ,"G28\n", "M0\n" };

std::string Print() {
   
    if (!requesttest.empty()) {
        auto target = requesttest.front();
        requesttest.erase(requesttest.begin());
        return target;
   }
    return "";
    

}

int main(){
	std::cout<<"hello cmake test"<<std::endl;

  Host host;
  host.Register(Print);
 
  /*
  host.writing("G0 Y10\n");
  host.writing("G0 X10\n");
  host.writing("M106 S255\n");
  host.writing("M105\n");
  host.writing("G0 Z10\n");

  host.writing("G0 Y10\n");
  host.writing("G0 X10\n");
  host.writing("M106 S0\n");
  host.writing("G0 Z0\n");

  host.writing("G0 X180\n");
  host.writing("M106 S255\n");
  host.writing("G0 Z10\n");
  host.writing("G0 Y0\n");
  host.writing("G0 X50\n");

  */

  while (1) {



  }
  host.Stop();

 
	return 0;
}
