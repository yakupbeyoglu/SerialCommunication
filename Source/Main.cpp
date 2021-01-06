#include <iostream>
#include "SerialCom/Serial.h"
#include "SerialCom/Host.h"
#include <thread>

std::vector<std::string> requesttest = { "M105\n","","M106 S255\n"};
std::vector<std::string> requesttest2 = { "G0 Z10\n","G0 Z0\n","G0 Z10\n","M106 S0\n" };

std::string Print() {
   
    if (!requesttest.empty()) {
        auto target = requesttest.front();
        requesttest.erase(requesttest.begin());
        return target;
   }
    return "";
    

}
std::string Print2() {

    if (!requesttest2.empty()) {
        auto target = requesttest2.front();
        requesttest2.erase(requesttest2.begin());
        return target;
    }
    return "";
}

int main(){
	std::cout<<"hello cmake test"<<std::endl;

  Host host;
  host.Register(Print);
  host.Register(Print2);


  while (1) {
      if (requesttest.empty() && requesttest2.empty()) {
          host.Stop();
          break;
      }


  }

 
	return 0;
}
