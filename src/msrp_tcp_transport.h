#ifndef _MSRP_TCP_TRANSPORT
#define _MSRP_TCP_TRANSPORT 1

namespace msrp
{
  class TcpTransport : public Transport
  {
    public:
      TcpTransport();

      bool connect(Address &remoteAddress);
      bool listen(Address &localAddress);
      void close() = 0;

      int read(char *data, size_t count);
      int peek(char *data, size_t count);
      int write(char *data, size_t count);
  }
}

#endif
