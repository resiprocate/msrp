#include "TcpListener.h"
#include "Address.h"
#include "Stack.h"

msrp::TcpListener::TcpListener(Address &localAddress, int backlog)
  : mLocalAddress(localAddress), mBacklog(backlog)
{
  
}


msrp::TcpListener::~TcpListener()
{
  
  close();

}

void
msrp::TcpListener::close()
{

  ::close(mDescriptor);

}


bool
msrp::TcpListener::TcpListener()
{

  switch (localAddress.getAddressType()) {
    
  case msrp::Address::IPV4: 
    {
      Ipv4Address *addr = reinterpret_cast<Ipv4Address *>(&localAddress);
      int status;
      
      mDescriptor = socket(PF_INET, SOCK_STREAM, 0);
      if (mDescriptor == -1)
	{
	  mDescriptor = 0;
	  return false;
	}

      sockaddr_in sin;
      status = bind(s, &sin, sizeof(sin));
      
      if (status == -1) {
	mDescriptor = 0;
	return false;
      }
  
      // set non-blocking 
      int oldflags = fcntl (desc, F_GETFL, 0);
      if (oldflags == -1)
	return -1;
      if (value != 0)
	oldflags |= O_NONBLOCK;
      else
	oldflags &= ~O_NONBLOCK;
      fcntl(mDescriptor, F_SETFL, oldflags)

      listen (mDescriptor, mBacklog);

    }
  break;
  
  default:
    return false;

  }

  return true;
  
}


void 
msrp::TcpListener::process()
{

  int result, fromlen;
  struct sockaddr_in from;
  fromlen = sizeof(from); 
  
  result = accept(mDescriptor, (struct sockaddr *) &from, &fromlen);
  
  if (result == -1) {
    return false;
  }

  new TcpConnection(mStack, mDescriptor);
  
}

