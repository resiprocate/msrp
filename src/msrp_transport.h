#ifndef _MSRP_TRANSPORT
#define _MSRP_TRANSPORT 1

#include <sys/types.h>

namespace msrp
{
  class Address;
  class Transport
  {
    public:
      friend class TransportGroup;

      virtual ~Transport();

      virtual bool connect(Address &remoteAddress) = 0;
      virtual bool listen(Address &localAddress) = 0;
      virtual void close() = 0;

      virtual int read(char *data, size_t count) = 0;
      virtual int readFrom(char *data, size_t count, Address *&addr) = 0;

      virtual int peek(char *data, size_t count) { return -1; }
      virtual int peekFrom (char *data, size_t count, Address *&addr) 
                                                 { return -1; }

      virtual int write(char *data, size_t count) = 0;
      virtual int writeTo(char *data, size_t count, Address &addr) = 0;

    protected:
      Transport();

      /**
        This descriptor must be usable in e.g. a select
        (or similar) statement. There is no requirement that
        calling read() on this descriptor will actually have
        any useful effect. For descriptor-based transports,
        this can just be the actual descriptor for the
        underlying socket. For any transports that dont have
        such a descriptor, the transport object is responsbile
        for making sure that this descriptor will be flagged
        as having data available whenever data on the corresponding
        transport becomes available. In general, the easiest way
        to do this is to set up a pipe (e.g. using "pipe()") and
        writing a single byte into the pipe when data is available.

        I have verified that this scheme should work under Windows
        in addition to actual Posix-compliant operating systems.

        I also think this *should* work transparently with libevent.
       */
      int descriptor;

      Address *localAddress;
      Address *remoteAddress;
  };
}
#endif
