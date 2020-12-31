#pragma once
#include "Serial.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <iostream>
#include <string.h>
#include <sys/ioctl.h>
namespace Internal {
 struct serialdata {
         //connection file set
         int conection;
        // termios for configuration
        termios parameters;
 };
}

namespace SerialConnection {
 Serial::Serial(const ByteSize &size, const ConnectionMethod &method,
                const ParityCheck&parity, const StopBits stopbit) :
                bytesize(size), connectionmethod(method), parity(parity), stopbits(stopbit) {
                    data = new Internal::serialdata;


                    data->parameters.c_cflag   &= (parity == ParityCheck::Even) ? PARENB :
                                                 (parity == ParityCheck::Odd) ? PARODD : ~PARENB;

                    data->parameters.c_cflag &= (stopbit == StopBits::TwoStopBit) ? CSTOPB  : ~CSTOPB;
                    data->parameters.c_cflag &= ~CSIZE;
                    data->parameters.c_cflag |= CS8;
                    data->parameters.c_cflag |= CREAD  | CLOCAL;
                   data->parameters.c_cflag |= CRTSCTS;  // Enable RTS/CTS hardware flow control
                   data->parameters.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                                  | INLCR | IGNCR | ICRNL | IXON);
                   data->parameters.c_oflag &= ~OPOST;
                   data->parameters.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
                   data->parameters.c_cflag &= ~(CSIZE | PARENB);

                   data->parameters.c_cc[VTIME] = 10;
                   data->parameters.c_cc[VMIN] = 0;






std::cout<<"Created"<<std::endl;
 }

 Serial::~Serial() {
   if(isconnect)
     DisConnect();
  delete data;

 }

 bool Serial::OpenPort(std::string portname) {

   auto connectmethod = connectionmethod == ConnectionMethod::R ? O_RDONLY :
                         connectionmethod == ConnectionMethod::W ? O_WRONLY : O_RDWR;
   data->conection = open("/dev/ttyUSB0", connectmethod);

   if(data->conection == -1)
     return false;
   return true;
 }

 bool Serial::ConfigPort(){
   if(tcgetattr(data->conection, &data->parameters) == 1 ) {
        std::cout<<"Set default has error"<<std::endl;

     }

   if(tcsetattr(data->conection,TCSANOW, &data->parameters) == -1)
         {
            std::cout<<"Error on paramaters"<<std::endl;
            return false;
          }

        return true;
 }

 bool Serial::Connect(const std::string portname, const int &baudrate) {
   if(isconnect)
        return true;
     // baudrate settings
     cfsetispeed(&data->parameters,baudrate);
     cfsetospeed(&data->parameters,baudrate);
     std::string portpath = "/dev/"+portname;

     if(!OpenPort(portpath)) {
       std::cout<<"port error";
         return false;

       }


      if(!ConfigPort())
        return false;
      isconnect = true;

     return true;
 }

 bool Serial::Connect(const int &portnumber, const int &baudrate){
   if(isconnect)
     return true;
  // baudrate settings
  cfsetispeed(&data->parameters,baudrate);
  cfsetospeed(&data->parameters,baudrate);
  std::string portpath = "/dev/data->parameters"+std::to_string(portnumber);


  if(!OpenPort(portpath))
    return false;

  if(!ConfigPort())
    return false;
  isconnect = true;
  return true;

 }

 bool Serial::DisConnect(){
   if(!isconnect)
     return true;
  if(close(data->conection)!=-1){
      isconnect = false;
      return true;
   }
  return false;

 }


 int Serial::Read() {
    if(!isconnect)
      return 0;
    int sizeonbuffer;
    ioctl(data->conection,FIONREAD,&sizeonbuffer);
    char buffer[sizeonbuffer];
    memset(&buffer,'\0',sizeonbuffer);
    int n = read(data->conection,&buffer,sizeonbuffer);
    //ioctl(data->conection,TCFLSH,2);

return 0;
 }

 int Serial::Write(const std::string &text) {
   if(!isconnect)
     return 0;
   if(write(data->conection,text.c_str(),text.length())!= -1){
       return text.length();
     }

   return 0;


 }


}


