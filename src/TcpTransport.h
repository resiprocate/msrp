#ifndef _MSRP_TCP_TRANSPORT
#define _MSRP_TCP_TRANSPORT 1

#include "Transport.h"

namespace msrp
{
  class Address;

  class TcpTransport : public Transport
  {
    public:
      TcpTransport();
      TcpTransport(Address &remoteAddress);
      virtual ~TcpTransport();

      virtual bool connect(Address &remoteAddress);
      virtual bool listen(Address &localAddress);
      void close() = 0;

      virtual int read(char *data, size_t count);
      virtual int readFrom(char *data, size_t count, Address *&addr);

      virtual int peek(char *data, size_t count);
      virtual int peekFrom(char *data, size_t count, Address *&addr);

      virtual int write(char *data, size_t count);
      virtual int writeTo(char *data, size_t count, Address &addr);
  };
}

#endif
