#ifndef _MSRP_LISTENER
#define _MSRP_LISTENER 1

#include <sys/types.h>

#if defined(WIN32)
#include <Ws2tcpip.h>
#else
#include <netinet/in.h>
#endif

#include "Connection.h"
#include "resiprocate/os/Tuple.hxx"

namespace msrp
{

  class Address;
  class Stack;

  class Listener : public FDEntry
  {

  public:
    
    enum returnTypes {FAIL = -1};
    
    Listener(Stack &stk, unsigned short somePort );
    virtual ~Listener();
    
    virtual Connection* make(sockaddr &aSockaddr) =0;

    bool listen(Address &localAddress);

    
    Connection* accept(); 
    
    void close();
    
  private:
    
    Address *mLocalAddress;
    int mDescriptor;
    Stack *mStack;
    unsigned short mPort;

  };

}
#endif
