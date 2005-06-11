#ifndef _MSRP_LISTENER
#define _MSRP_LISTENER 1

#include <sys/types.h>

namespace msrp
{
  class Address;
  class Listener
  {
    public:

    Listener(stack *stk, Address &localAddress);
    virtual ~Listener();
    
    virtual void process() = 0; 
    
    virtual void close() = 0;
    
  private:
    
    Address *mLocalAddress;
    int mDescriptor;
    stack *mStack;

  };
}
#endif
