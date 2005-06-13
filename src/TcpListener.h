#ifndef _MSRP_TCPLISTENER
#define _MSRP_TCPLISTENER 1

#include <sys/types.h>
#include "Listener.h"


namespace msrp
{
  class Address;
  class TcpListener : public Listener
  {
    public:

    TcpListener(stack *stk, Address &localAddress, int backlog = 25);
    virtual ~Listener();


    virtual Connection* make(sockaddr &aSockaddr);

    
  private:
    
    int mBacklog;
    sockaddr_in sin;

  };
}
#endif
