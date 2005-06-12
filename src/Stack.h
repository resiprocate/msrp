#ifndef _MSRP_STACK
#define _MSRP_STACK 1

#include <map>

#include "Constants.h"
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
      unsigned long getTimeTillNextTimer() const;

      // check if timers have fired and dns results
      void process();

      // will check and see if we already have a Connection to this target
      Connection* connect(const Tuple& target);
      void onConnected(const Tuple& source, Connection* connection);
      
      void dnsBlacklist(const Data& name, const resip::Tuple& tuple);
      
      static void onConnectionCallback(int fd, short event, void* clientData);
      void onReadyToRead(Socket fd);
      void onReadyToWrite(Socket fd);
      
   private:
      void removeSession(Session* session)
      {
         
      }
      
      std::vector<Listener*> mListeners;
      msrp::TimerQueue mTimerQueue;
      msrp::DnsBlackLister mBlacklister;
      std::map<Socket, std::list<Session*> > mSocketToSessionMap;
      std::map<Tuple, Connection*> mConnectionMap;
};

};

#endif
