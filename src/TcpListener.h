#ifndef _MSRP_TCPLISTENER
#define _MSRP_TCPLISTENER 1

#include <sys/types.h>

namespace msrp
{
  class Address;
  class TcpListener
  {
    public:

    TcpListener(stack *stk, Address &localAddress, int backlog = 25);
    virtual ~Listener();
    
    virtual void process(); 
    
    virtual void close();
    
  private:
    
    int mBacklog;
    sockaddr_in sin;

  };
}
#endif
