#ifndef _MSRP_LISTENER
#define _MSRP_LISTENER 1

#include "msrp/Connection.h"
#include "resiprocate/os/Tuple.hxx"

namespace msrp
{

class Connection;
class Stack;

class Listener : public resip::Poll::FDEntry
{
   public:
      Listener(Stack& stk, unsigned short listenPort );
      virtual ~Listener();
    
      Connection* accept(); 
      void close();
      
      virtual void doRead(); // from resip::Poll::FDEntry
      
   protected:
      virtual Connection* make(sockaddr &aSockaddr) =0;
    
   private:
      Stack& mStack;
      resip::Tuple mTuple;
      int mDescriptor;
      unsigned short mPort;
      int mBackLog;
}; 

}
#endif
