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
             
        if (read)
            Stop();
       
    }
    void Stop() {
        read = false;
        hasread = false;
       // sync.notify_all();
        writet.join();

        readt.join();
    }

    

    void FunctionsManager() {
        lastfunction++;
        if (lastfunction > functions.size() - 1)
            lastfunction = 0;



    }
    std::string  Request() {
        FunctionsManager();
        std::string value;



        do {
            if (lastfunction >=functions.size())
                lastfunction = 0;
            
            value = functions[lastfunction]();
            lastfunction++;
            
        } while (value.empty());
        lastfunction++;
        return value;
    
       
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
                    hasread = false;
                }
            }

            sync.notify_all();




        }

    }


    void Parser(std::string &last) {
        bool isok = false;
        std::string value;
        for (auto &n : last) {
          
            if (isspace(n)) {

                if (value.find("ok") != std::string::npos) {
                    isok = true;
                    value.clear();
                }

                if (value.find("T:") != std::string::npos  && isok) {
                    value.erase(value.find("T:"), 2);
                    std::cout << "Nozzle = " << value;
                }
                if (value.find("B:") != std::string::npos && isok) {
                    value.erase(value.find("B:"), 2);
                    std::cout << "Bed = " << value << std::endl;
                }
                value.clear();
            }
            
                value.push_back(n);





        }


    }


    void Read() {
        
        while (read) {

     
            auto target = serial.Read();
 
            if (!target.empty()) {

                //auto z = target.find('\n');
                lastread.append(target);
                
               
                if (lastread.find("ok") != std::string::npos && lastread.find("\n") != std::string::npos) {
                    queue.push_back(lastread);
                    Parser(lastread);
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
    std::mutex m, n;
    std::condition_variable sync;
    bool hasread = false;
    std::thread readt, writet;
    std::string lastread;
    bool canwrite = true;
    std::vector<std::function<std::string()>> functions;
    int lastfunction = -1;


};


