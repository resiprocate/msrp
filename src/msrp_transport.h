#ifndef _MSRP_TRANSPORT
#define _MSRP_TRANSPORT 1

namespace msrp
{
  class Transport
  {
    public:
      Transport();

      virtual bool connect(Address &remoteAddress) = 0;
      virtual bool listen(Address &localAddress) = 0;
      virtual void close() = 0;

      int read(char *data, size_t count) = 0;
      int peek(char *data, size_t count) { return -1; }
      int write(char *data, size_t count) = 0;
  }
}

#endif
