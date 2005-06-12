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
      
      static void onConnectionCallback(int fd, short event, void* clientData)
      {
         // clientData -> Connection*
      }
      
      void onReadyToRead(Connection* conn)
      {
      }
      
      void onReadyToWrite(Connection* conn)
      {
         SocketMap::iterator s = mSocketToActiveSessionMap.find(fd);
         assert(s != mSocketToActiveSessionMap.end());

         SessionList& active = s->second;
         Session* session = active.front();
         active.pop_front();
         Transmit::Result result = session->transmit();
         if (result == Transmit::MoreData)
         {
            active.push_back(session);
         }
         else if (result == Transmit::Failed)
         {
            active.clear();
            SocketMap::iterator all = mSocketToSessionMap.find(fd);
            assert(all != mSocketToSessionMap.end());
            std::for_each(all->second.begin(), all->second.end(), Session::DeleteSession());
            all->second.clear();

         }
      }
      
   private:
      Connection* associateConnection(const Tuple& tuple, Session* session)
      {
         TupleMap::iterator i = mConnectionMap.find(tuple);
         Connection* conn = 0;
         if (i == mConnectionMap.end())
         {
            Connection* conn = new TcpConnection(*this, tuple);
            mConnectionMap[tuple] = conn;
         }
         else
         {
            conn = i->second;
         }
         mConnectionToSessionMap[conn].push_back(session);
      }
      
      void removeConnection(const Tuple& tuple)
      {
         // Connection should delete himself when failure occurs
         mConnectionMap.erase(tuple);
      }
      
      void removeSession(Session* session)
      {
         
      }
      
      std::vector<Listener*> mListeners;
      msrp::TimerQueue mTimerQueue;
      msrp::DnsBlackLister mBlacklister;

      typedef std::list<Session*> SessionList;
      typedef std::map<Connection*, SessionList> ConnectionMap;
      ConnectionMap mConnectionToSessionMap;
      ConnectionMap mConnectionToActiveSessionMap;
      
      typedef std::map<Tuple, Connection*> TupleMap;
      TupleMap mConnectionMap;
};

};

#endif
