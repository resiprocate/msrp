#if !defined(MSRP_CONNECTION_BASE_HXX)
#define MSRP_CONNECTION_BASE_HXX 

#include <list>

namespace msrp
{

class TransactionMessage;

class ByteWrangler
{
   public:
      ByteWrangler(Stack& s);
      
      enum Result
      {
         Success,
         ScannerFailure
      };

   protected:
      enum State
      {
         NewMessage = 0,
         ReadingHeaders,
         MidBody,
         TailReached
         MAX
      };
	
      State getCurrentState() const { return mState; }
      void preparseNewBytes(int bytesRead, Fifo<TransactionMessage>& fifo);
      std::pair<char*, size_t> getWriteBuffer();
	 
      // for avoiding copies in external transports--not used in core msrp
      void setBuffer(char* bytes, int count);

      Data::size_type mSendPos;

      ByteWrangler();
      virtual ~ByteWrangler();
      // no value semantics
      ByteWrangler(const ByteWrangler&);
      ByteWrangler& operator=(const ByteWrangler&);
   private:
      MsrpMessage* mMessage;
      char* mBuffer;
      size_t mBufferPos;
      size_t mBufferSize;
  
      static char connectionStates[MAX][32];
      UInt64 mLastUsed;
      State mState;
      MsgHeaderScanner mMsgHeaderScanner;
  
  
};

std::ostream& 
operator<<(std::ostream& strm, const msrp::ByteWrangler& c);

}

#endif
