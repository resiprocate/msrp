#include "Listener.h"
#include "Address.h"
#include "Stack.h"

msrp::Listener::Listener(Stack *stk, unsigned short somePort)
    : mLocalAddress(0), mStack(stk), mDescriptor(0),mPort(somePort)
{}

msrp::Listener::~Listener()
{}


msrp::Listener::~Listener()
{
  
  close();

}


void
msrp::Listener::close()
{

  ::close(mDescriptor);

}

Connection* 
msrp::Listener::accept()
{

  int result, fromlen;
  struct sockaddr_in from;
  fromlen = sizeof(from); 
  
  result = accept(mDescriptor, (struct sockaddr *) &from, &fromlen);

  if (result == -1) {
    return false;
  }

  Tuple tup(from);


  //new TcpConnection(mStack, mDescriptor);
  return make((sockaddr)from );
  
}


