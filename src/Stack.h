#ifndef _MSRP_STACK
#define _MSRP_STACK 1

#include <map>

#include "Constants.h"
#include "Callback.h"
#include "Session.h"

namespace msrp
{

class Stack 
{
   public:
      Stack(char *localName);
      typedef enum 
      {
         Tls, 
         Tcp
      } TransportType;
      
      class Exception : public BaseException
      {
         public:
            BaseException(const char* msg, const char* file, int line);
            virtual const char* name() const = 0;
      };
         
      // Will throw Stack::Exception if port is inuse
      void addTransport(TransportType type, int port=11111);
      int getTimeTillNextTimer() const;
      
   private:
      std::vector<Listener*> mListeners;
      msrp::TimerQueue mTimerQueue;
      
      std::map<unsigned short, Socket> sockets;
}
};

#endif
