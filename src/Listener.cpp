#include "Listener.h"
#include "Address.h"

msrp::Listener::Listener(stack *stk, Address &localAddress)
  : mLocalAddress(localAddress), mStack(stk)
{
  
  
  
}

msrp::Listener::~Listener()
{
  delete mLocalAddress;
}
