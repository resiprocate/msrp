#ifndef _MSRP_CONNECTION
#define _MSRP_CONNECTION 1

#include <sys/types.h>
#include "MsrpRoar.h"

namespace msrp
{
  class Tuple;
  class Stack;

  // change the Address to be tuple
  // need accessors for the tuples

  class Connection
  {

    enum ReturnTypes {FAIL = -1};
    enum ConnFlag {DONE, CONTINUE, DEAD};
    
    public:

      friend class ConnectionGroup;
      friend class Listener;

      virtual ~Connection();
      
      virtual bool connect(Tuple &remoteTuple) = 0;
      virtual void close() = 0;
      
      virtual int transmit(const MsrpRoar&, char *, int chunkLen,
			   ConnFlag); 

      void processReads();
      
    protected:

      virtual int write(char *data, size_t count) = 0;

      virtual int read(char *data, size_t count) = 0;

      Connection(Stack *);

      int mDescriptor;
      Stack *mStack;
      
      Tuple *mRemoteTuple;
      Tuple *mLocalTuple;

      Data mTID;
      Data mRoarBuf;
      Data mTailBuf;
      bool mToWrite;

      ConnFlag mFlag;
      

      int mRoarCount;
      int mTailCount;

      char *mCursor;



      char *mInsert;
      int mBufSz;
      


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
};


}
#endif
