#include <sys/types.h>
#include <sys/socket.h>
#include "msrp_tcp_transport.h"
#include "msrp_address.h"

msrp::TcpTransport::TcpTransport()
{
}

msrp::TcpTransport::TcpTransport(Address &remoteAddress)
{
  connect(remoteAddress);
}


bool
msrp::TcpTransport::connect(Address &remoteAddress)
{
  if (descriptor)
  {
    return 0;
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
    }
    break;

    default:
      return false;    
  }
  return true;
}
