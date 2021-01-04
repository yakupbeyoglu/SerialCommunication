#include "Serial.h"
#include <thread>
#include <mutex>
class Host {
public:
    Host() {
        
        serial.Connect(3, 115200);
        read = true;
    
     
    }

    void Stop() {
        read = false;
        hasread = true;
        sync.notify_all();
        writesync.notify_all();
        haswrite = true;
    }

    void QueueManager(std::string data) {
        std::unique_lock<std::mutex> managelock{ m };

        //std::lock_guard<std::mutex> lock{ m };
            code = data;
            

            hasread = true;
            sync.notify_all();

            sync.wait(managelock, [&] {return haswrote; });

            auto z = Get();
            if (!z.empty()) {
                std::cout << "Readen :" << std::endl;
                std::cout << z << std::endl;
                haswrite = true;
                writesync.notify_all();

            }

            
            sync.wait(managelock, [&] {return !haswrote; });



    }

    void Read() {
        std::unique_lock<std::mutex> readlock{ m };

        while (read) {

            sync.wait(readlock, [&] {return hasread; });
     
            auto target = serial.Read();
 
            if (!target.empty()) {
                queue.push_back(target);
                hasread = false;
                haswrote = true;
                sync.notify_all();
            }
            else
                sync.notify_all();
         

        }
    }
    

     void Write() {
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
       /*  do {

             z = Get();
             if (!z.empty()) {
                 std::cout << z;
                 serial.Write(gcode);

             }
         } while (z.empty());*/
     }

     //std::thread workloop() {
   //      return std::thread(&Host::QueueManager, this);
     //}
    
     std::thread reading() {
         return std::thread(&Host::Read, this);
     }

     std::thread writing() {
         return std::thread(&Host::Write, this);
         
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


private:
    bool read = false;
    SerialConnection::Serial serial;
    std::vector<std::string> queue;
    std::mutex m, n;
  //  std::thread workt,writet,readt;
    // sync each thread with notify
    std::condition_variable sync, writesync;
    bool hasread = false, haswrite = false, haswrote = false;
    std::string code = "";


};


