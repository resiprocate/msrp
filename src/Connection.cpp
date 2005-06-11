#include "Connection.h"
#include "Address.h"

using namespace msrp;


msrp::Connection::Connection(Stack *stk)
  : mDescriptor(0), mStack(stk), mLocalAddress(0), mRemoteAddress(0)
{
}


Connection::~Connection()
{
  delete mLocalAddress;
  delete mRemoteAddress;
}

  
void 
Connection::processReads()
{
   // see if there is any data to read from the network

   // pass it to the byte wrangler

   // see if the byte rangler has produced a Roar or some body Data 

   // pass the data to the corect OutgoingMessage 
}
