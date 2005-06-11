#include "Connection.h"
#include "Address.h"


msrp::Connection::Connection(Stack *stk)
  : mDescriptor(0), mStack(stk), mLocalAddress(0), mRemoteAddress(0)
{
}

msrp::Connection::~Connection()
{
  delete mLocalAddress;
  delete mRemoteAddress;
}
