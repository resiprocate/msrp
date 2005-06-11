#ifndef _MSRP_LISTENER
#define _MSRP_LISTENER 1

#include <sys/types.h>

namespace msrp
{

  class Address;
  class Stack;

  class Listener
  {

  public:
    
    enum returnTypes {FAIL = -1};
    
    Listener(Stack *stk);
    virtual ~Listener();
    
    virtual bool listen(Address &localAddress) = 0;

    
    virtual bool process() = 0; 
    
    virtual void close() = 0;
    
  private:
    
    Address *mLocalAddress;
    int mDescriptor;
    Stack *mStack;

  };

}
#endif
