#ifndef _MSRP_SESSION
#define _MSRP_SESSION 1

#include "Constants.h"
#include "Callback.h"
#include <list>

namespace msrp
{
class Session : public DnsResult
{
   public:

      // We need to send an offer - the offerer
      // never listens, but needs to bind to a local port.
      Session(Stack& stack,
              std::vector<msrp::RelayRecord> relays
              = std::vector<msrp::RelayRecord>());

      // We have an offer, and need to send an answer
      Session(Stack& stack,
              const char* toPath,
              std::vector<msrp::RelayRecord*> relays
              = std::vector<msrp::RelayRecord*>(),
              int listenPort = PREFERRED_PORT);

      struct DeleteOutgoingMessage : std::unary_function<OutgoingMessage*,void>
      {
            void operator()(OutgoingMessage* msg)
            {
               delete msg;
            }
      };
      
      ~Session()
      {
         mStack.removeSession(this);
         std::for_each(mOutgoingMessages.begin(), mOutgoingMessages.end(), DeleteOutgoingMessage());
      }
      
      void addRemotePath(char *toPath);
 
      std::string getPath() const;

      unsigned short getLocalPort();

      // Callbacks

      virtual IncomingMessage* onNewMessage()
      {
      }
      
      
      // the application has provided data
      virtual bool hasDataToSend() const
      {
         return !mPendingTransmits.empty();
      }

      // From msrp::DnsResult
      virtual void onResolved(const Data& name, const resip::Tuple& tuple) const
      {
         assert(mConnection == 0);
         mConnection = mStack.connect(tuple);
      }
      
      virtual void onFailed(const Data& name) const
      {
         // ?jf? should we blacklist here or do something useful?
         delete this;
      }
      
   private:
      // called by Stack::onReadyToWrite and assumes that there's data pending
      // to write from the app. calls OutgoingMessage::transmit using
      // roundrobin on the mPendingTransmits. If OutgoingMessage::transmit
      // returns Done, remove from mPendingTransmits
      void transmit()
      {
         assert(!mPendingTransmits.empty());
         OutgoingMessage* msg = mPendingTransmits.front();
         mPendingTransmits.pop_front();
         if (msg->transmit() == OutgoingMessage::MoreData)
         {
            mPendingTransmits.push_back(msg);
         }
         else if (msg->transmit() == OutgoingMessage::Failed)
         {
            mStack.removeSession(this);
            
         }
      }

      void addToPendingTransmit(OutgoingMessage* msg)
      {
         if (!mConnection)         
         {
            if (mPendingTransmits.empty())
            {
               mResolver.resolve(mNextHop);
            }
         }
         mPendingTransmits.push_back(msg);
      }
      
      msrp::Stack& mStack;
      msrp::DnsResolver mResolver;
      std::vector<msrp::RelayRecord> mRelays;
      resip::Data mNextHop;
      msrp::Connection* mConnection;
      bool mTheRemotePathIsSet;
      unsigned short mLocalPort;
      std::vector<std::string> mPath;

      // OutgoingMessage* gets placed here when app calls OutgoingMessage::send
      // or OutgoingMessage::abort.
      std::list<OutgoingMessage*> mPendingTransmits;
      std::list<OutgoingMessage*> mOutgoingMessages;
}
};

#endif
