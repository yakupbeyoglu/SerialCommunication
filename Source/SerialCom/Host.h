#include "Serial.h"
#include <thread>
#include <mutex>

struct Temperature {
    float nozzle, bed;
};


class Host {

public:

    Host() {
        Connect();
     
    }

    void Register(const std::function<std::string()> &f) {
        functions.push_back(f);
   }

    
    void UnRegister(const int &index) {
        if (index < functions.size())
            functions.erase(functions.begin() + index);
    }
    
    void UnRegisterAll() {
        urgentfunctions.clear();
        functions.clear();
    }

    void UrgentRegister(const std::function<std::string()> &f) {
        hasurgent = true;
        urgentfunctions.push_back(f);
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

        std::string value="";

        if (hasurgent) {
            for (auto &urgent : urgentfunctions) {
                value = urgent();
                if (!value.empty())
                    return value;
            }

        }

        if (value.empty()) {

            FunctionsManager();



            do {
                if (lastfunction >= functions.size())
                    lastfunction = 0;

                value = functions[lastfunction]();
                lastfunction++;

            } while (value.empty());
            lastfunction++;
            return value;

        }
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
          
            if (value.find("Error:") != std::string::npos) {
                errormessage = value;
            }
            else {
                if (isspace(n)) {

                    if (value.find("ok") != std::string::npos) {
                        isok = true;
                        value.clear();
                    }

                    if (value.find("T:") != std::string::npos  && isok) {
                        value.erase(value.find("T:"), 2);
                        temperature.nozzle = std::stof(value);
                    }

                    if (value.find("B:") != std::string::npos && isok) {
                        value.erase(value.find("B:"), 2);
                        temperature.bed = std::stof(value);
                    }


                    value.clear();
                }
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
                
                // both control seperated because of the  temeprature return 
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
            serial.Write(text);
            std::cout << "writed code  = " << text;
        }
    
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
    std::vector<std::function<std::string()>> urgentfunctions;

    int lastfunction = -1;
    Temperature temperature;
    std::string errormessage;
    bool hasurgent;
};


