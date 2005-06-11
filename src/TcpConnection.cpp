#include <sys/types.h>
#include <sys/socket.h>
#include "TcpConnection.h"
#include "Address.h"
#include "Stack.h"

msrp::TcpConnection::TcpConnection()
{
  
}

msrp::TcpConnection::TcpConnection(stack *stk, Address &remoteAddress)
  : mStack(stk)
{
  
  connect(remoteAddress);

}


msrp::TcpConnection::TcpConnection(stack *stk, int fd)
  : mStack(stk), mDescriptor(fd)
{}


msrp::TcpConnection::~TcpConnection(stack *stk)
  : mStack(stk)
{}



bool
msrp::TcpConnection::connect(Address &remoteAddress)
{
  if (descriptor)
  {
    return false;
  }

  switch (remoteAddress.getAddressType())
  {
    case msrp::Address::IPV4:
    {
      Ipv4Address *addr = reinterpret_cast<Ipv4Address *>(&remoteAddress);
      int status;
     
      descriptor = socket(PF_INET, SOCK_STREAM, 0);
      if (descriptor == -1)
      {
        descriptor = 0;
        return false;
      }

      status = ::connect(descriptor, *addr, sizeof(struct sockaddr_in));
      if (status == -1)
	{
	  descriptor = 0;
	  return false;
	}
    }
    break;

    default:
      return false;    
  }
  return true;
}


int
msrp::TcpConnection::write(char *data, size_t count)
{

  if (!descriptor) {
    return msrp::Connection::FAIL;
  }
    
  int n;
  
  n = ::write(descriptor, data, count);
  
  if (n == -1) {
    return msrp::Connection::FAIL;
  }

  return n;

}


int
msrp::TcpConnection::read(char *data, size_t count)
{

  if (!descriptor) {
    return msrp::Connection::FAIL;
  }

  int n;
  
  n = ::read(descriptor, data, count);
  
  if (n == -1) {
    return msrp::Connection::FAIL;
  }

  return n;

}
