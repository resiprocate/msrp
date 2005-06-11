#include "Connection.h"
#include "Address.h"

msrp::Connection::Connection()
  : descriptor(0), localAddress(0), remoteAddress(0)
{
}

msrp::Connection::~Connection()
{
  delete mLocalAddress;
  delete mRemoteAddress;
}
