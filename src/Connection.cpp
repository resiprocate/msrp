#include "msrp/Connection.h"

using namespace msrp;

Connection::Connection(Stack& stack) : 
   mDescriptor(0), 
   mStack(stack), 
   mLocalAddress(0),
   mRemoteAddress(0), 
   mBufSz(0), 
   mStat(DONE)
{
}

Connection::~Connection()
{
   delete mLocalAddress;
   delete mRemoteAddress;
}
  
void 
Connection::doRead()
{
   // see if there is any data to read from the network

   // pass it to the byte wrangler

   // see if the byte rangler has produced a Roar or some body Data 

   // pass the data to the corect OutgoingMessage 
}



void 
Connection::doWrite()
{
   if (writeBuf())
   {
      return;
   }
  
   // !DAB register again, still not done writing
  
}



bool 
Connection::writeRoar(const MsrpRoar &roar)
{

   mTID = roar.getTransactionId();

   // I think this will get the the encoded roar in mRoarBuf
   Data encoded;
   DataStream str(mBuf);
   roar.encode(str);
   str.flush();

   mBufCount = mBuf.len();
   mCursor = mBuf.data();

   int wrote = write(mCursor, mBufCount);
   if (wrote == mBufCount) {
      return true;
   } else {
      assert (wrote <= mBufCount);
      mBufCount -= wrote;
      mCursor += wrote;
      // didn't get out the whole roar
      return false;
   }

}


int
Connection::writeBody(char *chunk, int chunkLen)
{

   // !DAB NEED TO CHECK FOR ---- stuff here
   // Cullen's suggestion is set ---- to a 32 bit int and do the 
   // comparison that way -- need to ensure the sizes of ints and such
   // work for that
   // *may* be odd cases (statistically unlikely) where we get some dashes
   // onto the pipe, fail and so return in INBODY, then get called from 
   // another sending body that has more dashes, the sum of which *could* 
   // be 7 dashes. Darn unlikely, but could happen...

   int wrote = write(chunk, chunkLen);
   if (wrote == chunkLen) {
      return chunkLen;
   }
   return wrote;

}


bool 
Connection::writeTail()
{

   mBuf = "-------" + mTID;
  
   switch (mFlag) {
    
      case DONE:
         mBuf += "$";
         break;
      case CONTINUE:
         mBuf += "+";
         break;
      case DEAD:
         mBuf += "#";
         break;
    
   }

   mBufCount = mBuf.len();
   mCursor = mBuf.data();
 
   wrote = write(mCursor, mTailCount);
   if (wrote == mBufCount) {
      return true;
   } else {
      assert (wrote <= mBufCount);
      mBufCount -= wrote;
      mCursor += wrote;
      mState = INTAIL;
      return false;
   }

}


bool 
Connection::writeBuf()
{

   // this is only tail or roar, never body, so no need to check for ----

   wrote = write(mCursor, mBufCount);
   if (wrote == mBufCount) {
      return true;
   } else {
      // should already be in an "IN" state
      assert (wrote <= mBufCount);
      mBufCount -= wrote;
      mCursor += wrote;
      return 0;
   }


}


int 
Connection::transmit(const MsrpRoar &roar, char *chunk,
                     int chunkLen, ConnFlag flag)
{

   int len;
  
   switch(mFlag) {

      case DONE:

         // no state currently
         mTID = roar.getTransactionId();
         mFlag = flag;

         // try the roar
         if (!writeRoar()) {
            mState = INROAR;
            return 0;
         }
    
         // try the body
         len = writeBody(chunk, chunkLen);
         if (len != chunkLen) {
            mState = INBODY;
            return len;
         }

         // try the tail
         if (!writeTail()) {
            mState = INTAIL;

            // !DAB NEED TO DO THE CALLBACK TO THE STACK TO REGISTER WE NEED TO
            // BE PROCESSED AGAIN LATER TO FINISH THE TAIL!!!!
         }
    
         // state is still done
         return len;
         break;

      case INROAR:

         // need to write out rest of buffer (with roar) no matter what
         if (!writeBuf()) {
            // still not done, still in INROAR
            return 0;
         }
    
         if (mTID = roar.getTransactionId()) {
      
            // same transaction, roar is written, can try to write the data
            len = writeBody(chunk, chunkLen);
            if (len != chunkLen) {
               mState = INBODY;
               return len;
            }
    
            // try the tail
            if (!writeTail()) {
               mState = INTAIL;
	
               // !DAB NEED TO DO THE CALLBACK TO THE STACK TO REGISTER WE NEED TO
               // BE PROCESSED AGAIN LATER TO FINISH THE TAIL!!!!
            }
    
            // state is now done
            mState = DONE;
            return len;

         } else { // different TID

            // finish what we have to do, and return 0 (could try to process the
            // new message here I suppose, but return 0 it will try again...

            // try the tail
            if (!writeTail()) {
               mState = INTAIL;
	
               // !DAB NEED TO DO THE CALLBACK TO THE STACK TO REGISTER WE NEED TO
               // BE PROCESSED AGAIN LATER TO FINISH THE TAIL!!!!
            }
      
            mState = DONE;
            return 0;
      
         }

         break;

      case INBODY:
    
         if (mTID = roar.getTransactionId()) {
      
            // same transaction, roar is written, can try to write the data
            len = writeBody(chunk, chunkLen);
            if (len != chunkLen) {
               mState = INBODY;
               return len;
            }
    
            // try the tail
            if (!writeTail()) {
               mState = INTAIL;
	
               // !DAB NEED TO DO THE CALLBACK TO THE STACK TO REGISTER WE NEED TO
               // BE PROCESSED AGAIN LATER TO FINISH THE TAIL!!!!
            }
    
            // state is now done
            mState = DONE;
            return len;

         } else { // different TID

            // finish what we have to do, and return 0 (could try to process the
            // new message here I suppose, but return 0 it will try again...

            // try the tail
            if (!writeTail()) {
               mState = INTAIL;
	
               // !DAB NEED TO DO THE CALLBACK TO THE STACK TO REGISTER WE NEED TO
               // BE PROCESSED AGAIN LATER TO FINISH THE TAIL!!!!
            }
      
            mState = DONE;
            return 0;
      
         }

         break;
    
      case INTAIL:
    
         // need to write out rest of buffer (with tail)
         if (!writeBuf()) {
            // still not done, still in INTAIL
            return 0;
         }

         // again, just write the incomplete transaction, then return 0
         // and let them try again in theory, we could try to do the new one
    
         mState = DONE;
         return 0;

         break;

   }

}


  
  
  
    
