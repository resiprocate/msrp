#include "msrp_transport.h"
#include "msrp_address.h"

msrp::Transport::Transport()
  : descriptor(0), localAddress(0), remoteAddress(0)
{
}

msrp::Transport::~Transport()
{
  delete localAddress;
  delete remoteAddress;
}
