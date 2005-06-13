#ifndef _MSRP_STACK
#define _MSRP_STACK 1

#include <map>

#include "Constants.h"
#include "Session.h"
#include "resiprocate/os/Poll.hxx"

namespace msrp
{

class Stack : public resip::Poll
{
   public:
      class Exception : public BaseException
      {
         public:
            BaseException(const char* msg, const char* file, int line) : 
               BaseException(msg,file,line)
            {
            }
            
            virtual const char* name() const
            {
               return "Msrp::Stack::Exception";
            }
      };

   public:
      Stack(char *localName) : mLocalName(localName)
      {
      }
      
      unsigned long getTimeTillNextTimer() const
      {
         return 0L;
      }

      // check if timers have fired and dns results
      virtual void process(int ms)
      {
         const std::vector<Poll::FDEntry *>& results = mPoll.wait(ms);
         
      }
      
      void process(const Connection* conn, const MsrpRoar& roar)
      {
         SessionMap::iterator u = mUnassociatedSessions.find(roar.getSessionId());
         if (u != mUnassociatedSessions.end())
         {
            Session* unassociated = u->second;
            mUnassociatedSessions.erase(u);
            mConnectionToSessionMap[conn].push_back(unassociated);
            unassociated->process(roar);
         }
         else
         {
            ConnectionMap::iterator m = mConnectionToSessionMap.find(conn);
            if (m != mConnectionToSessionMap.end())
            {
               SessionList& slist = m->second;
               for (SessionList::iterator s=slist.begin(); s != slist.end(); ++s)
               {
                  if (s->getId() == roar.getSessionId())
                  {
                     s->process(roar);
                     break;
                  }
               }
            }
            else
            {
               // send 481
               assert(0);
            }
         }
      }
      
      void dnsBlacklist(const Data& name, const resip::Tuple& tuple)
      {
         mBlacklister.blacklist(name, tuple);
      }
      
      static void onConnectionCallback(int fd, short event, void* clientData)
      {
         // this was the libevent callback. 
         // clientData -> Connection*
      }
      
      void onReadyToRead(Connection* conn)
      {
         conn->processReads();
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
      virtual Buffer& requestBuffer()
      {
         return mDefaultBuffer;
      }
      
      Listener* getListener(int port)
      {
         ListenerMap::iterator i=mListenerMap.find(port);
         if (i == mListenerMap.end())
         {
            mListenerMap[port] = new TcpListener(*this, port);
         }
         else
         {
            return i->second;
         }
      }
      
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
      
   private:
      const Data mLocalName;
      Buffer mDefaultBuffer;
      
      resip::Poll mPoll;
      
      //msrp::TimerQueue mTimerQueue;
      msrp::DnsBlackLister mBlacklister;
      
      typedef std::map<Data, Session*> SessionMap; // SessionId -> Session*
      SessionMap mUnassociatedSessions;
      
      typedef std::list<Session*> SessionList;
      typedef std::map<Connection*, SessionList> ConnectionMap;
      ConnectionMap mConnectionToSessionMap;
      ConnectionMap mConnectionToActiveSessionMap;

      // maps from listener port to Listener*
      typedef std::map<int, Listener*> ListenerMap;
      ListenerMap mListenerMap;
      
      // maps from peer address to Connection*
      typedef std::map<Tuple, Connection*> TupleMap;
      TupleMap mConnectionMap;
};

};

#endif
