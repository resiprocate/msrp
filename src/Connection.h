#ifndef _MSRP_CONNECTION
#define _MSRP_CONNECTION 1

#include <sys/types.h>

namespace msrp
{
class Address;
class Stack;

class Connection
{
   public:
      friend class ConnectionGroup;
      
      virtual ~Connection();
      
      virtual bool connect(Address &remoteAddress) = 0;
      virtual void close() = 0;
      
      virtual int write(char *data, size_t count) = 0;
      
      void processReads();
      
   private:        
      virtual int read(char *data, size_t count) = 0;
      
   protected:
      Connection(Stack *);
      
      /**
         This descriptor must be usable in e.g. a select
         (or similar) statement. There is no requirement that
         calling read() on this descriptor will actually have
         any useful effect. For descriptor-based connections,
         this can just be the actual descriptor for the
         underlying socket. For any connections that dont have
         such a descriptor, the connection object is responsbile
         for making sure that this descriptor will be flagged
         as having data available whenever data on the corresponding
         connection becomes available. In general, the easiest way
         to do this is to set up a pipe (e.g. using "pipe()") and
         writing a single byte into the pipe when data is available.
         
         I have verified that this scheme should work under Windows
         in addition to actual Posix-compliant operating systems.
         
         I also think this *should* work transparently with libevent.
      */
      int mDescriptor;
      
      stack *mStack;
      
      Address *mLocalAddress;
      Address *mRemoteAddress;
};

}
#endif
