#include "Serial.h"
#include <thread>
#include <mutex>

class Host {
public:
    Host() {
        Connect();
     
    }

    void Register(const std::function<std::string()> &f1) {
        functions.push_back(f1);
   }

    bool Connect() {
        if (!serial.IsConnect()) {
            serial.Connect(3, 115200);
            read = true;
            if (serial.IsConnect()) {
                readt = std::thread(&Host::Read, this);
                writet = std::thread(&Host::HostManager, this);
            }
            return true;
        }
        return false;
    }

    bool Disconnect() {

        Stop();

    }

    ~Host() {
      

        //readt->join();
        //writet->join();
       
    }
    void Stop() {
       // read = false;
        hasread = true;
        sync.notify_all();
        writesync.notify_all();
        haswrite = true;
        readt.join();
        writet.join();
    }

    

    std::string  Request() {
       
        std::string value;
        for (auto &function : functions) {
             value = function();
            if (!value.empty())
            {
                return value;
            }
        }

      //  &functions.back().operator();

        return "";

    }

    void HostManager() {
        // lock current threat
        std::unique_lock<std::mutex> managelock{ m };


        while (read) {
            // hasread = true;
            // sync.notify_all();

            sync.wait(managelock, [&] {return hasread; });
            std::string z = "";
            bool isvalid;

            if (!queue.empty()) {
                z = Get();
                isvalid = Validate(z);
                if (isvalid) {
                    auto code = Request();
                    if(!code.empty())
                        Write(code);
                   // below can work for resend
                   // sync.wait(managelock, [&] {return haswrote; });
                    //haswrote = false;
                }
            }

            sync.notify_all();




        }

    }

            
          //  sync.wait(managelock, [&] {return !haswrote; });



    

    void Read() {
        // lock read threat
     //  std::unique_lock<std::mutex> readlock{ m };

        while (read) {

        //    sync.wait(readlock, [&] {return hasread; });
     
            auto target = serial.Read();
 
            if (!target.empty()) {

                //auto z = target.find('\n');
                lastread.append(target);
                
               
                if (lastread.find("ok\n") != std::string::npos) {
                    queue.push_back(lastread);
                    hasread = true;
                    sync.notify_all();
                    lastread.clear();
                }

            }
        

        }
    }
    

    void Write(const std::string text) {
    
        std::unique_lock<std::mutex> lock{ n };
        int writebyte;
        if (!text.empty()) {
            if (text == "M0\n") {
                std::cout << "m0 geldi" << std::endl;

            }
            serial.Write(text);
            std::cout << "writed code  = " << text;
        }
    

      //  }
    }
    




     std::string Get() {
         std::unique_lock<std::mutex> lock{ n };
         std::string last;
         if (queue.size() > 0) {
             std::cout << queue.front() << std::endl;
             last = queue.front();
            queue.erase(queue.begin());
         }
       
         return last;
            
     }

     bool Validate(const std::string &txt) {
        
         if (txt.find("ok") != std::string::npos) 
             return true;
         

         return false;
     }


private:



    bool read = false;
    SerialConnection::Serial serial;
    std::vector<std::string> queue;
    std::vector<std::string> writequeue;
    std::mutex m, n;
    std::condition_variable sync, writesync;
    bool hasread = false, haswrite = false, haswrote = false;
    std::string code = "";
    std::thread readt, writet;
    std::string lastread;
    bool canwrite = true;
    std::vector<std::function<std::string()>> functions;

    std::vector<std::string> requesttest = {"G0 X20\n","G0 Y10\n","M106 S255\n","G0 Z10\n","M106 S0\n","G0 Y0\n" ,"G28\n", "M0\n" };

};


