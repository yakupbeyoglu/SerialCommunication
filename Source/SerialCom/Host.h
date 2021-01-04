#include "Serial.h"
#include <thread>
#include <mutex>

class Host {
public:
    Host() {
        serial.Connect(3, 115200);
        read = true;
        if (serial.IsConnect()) {
            readt = new  std::thread(&Host::Read, this);
            writet = new  std::thread(&Host::Write, this);
        }
     
    }

    bool Connect() {
        if (!serial.IsConnect()) {
            serial.Connect(3, 115200);
            read = true;
            if (serial.IsConnect()) {
                readt = new  std::thread(&Host::Read, this);
                writet = new  std::thread(&Host::Write, this);
            }
            return true;
        }
        return false;
    }

    bool Disconnect() {

        Stop();

    }

    ~Host() {
        readt->join();
        writet->join();
        delete readt;
        delete writet;
    }
    void Stop() {
        read = false;
        hasread = true;
        sync.notify_all();
        writesync.notify_all();
        haswrite = true;
        readt->join();
        writet->join();
    }

    void QueueManager(std::string data) {
        // lock current threat
        std::unique_lock<std::mutex> managelock{ m };

            code = data;
            

            hasread = true;
            sync.notify_all();

            sync.wait(managelock, [&] {return haswrote; });
            std::string z = "";
            bool isvalid;
            do {

                z = Get();
                isvalid = Validate(z);



            } while (!isvalid);
            if (!z.empty()) {
                std::cout << "Readen :" << std::endl;
                std::cout << z << std::endl;

                haswrite = true;
                writesync.notify_all();

            }


            
            sync.wait(managelock, [&] {return !haswrote; });



    }

    void Read() {
        // lock read threat
        std::unique_lock<std::mutex> readlock{ m };

        while (read) {

            sync.wait(readlock, [&] {return hasread; });
     
            auto target = serial.Read();
 
            if (!target.empty()) {
                queue.push_back(target);
                if (Validate(target)) {
                    hasread = false;
                    haswrote = true;
                    sync.notify_all();
                }

            }
            else
                sync.notify_all();
         

        }
    }
    

    void Write() {
        // lock write threat
        std::unique_lock<std::mutex> locking{ m };

        while (read) {
            writesync.wait(locking, [&] {return haswrite; });
            std::cout << "writed code  = " << code;
            serial.Write(code);
            code.clear();
            haswrite = false;
            haswrote = false;
            // locking.unlock();
            sync.notify_all();
        }
    }
    




     std::string Get() {
         //std::unique_lock<std::mutex> lock{ m };
         std::string last = "";
         if (queue.size() > 0)
             last = queue.back();
         if (!queue.empty())
             queue.pop_back();
         return last;
            
     }

     bool Validate(const std::string &txt) {
        
         if (txt.find("ok") != std::string::npos) 
             return true;
         

         return false;
     }


private:

    std::thread reading() {
        return std::thread(&Host::Read, this);
    }

    std::thread writing() {
        return std::thread(&Host::Write, this);

    }

    bool read = false;
    SerialConnection::Serial serial;
    std::vector<std::string> queue;
    std::mutex m, n;
    std::condition_variable sync, writesync;
    bool hasread = false, haswrite = false, haswrote = false;
    std::string code = "";
    std::thread *readt, *writet;


};


