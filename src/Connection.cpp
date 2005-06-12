#include "Connection.h"
#include "Address.h"

using namespace msrp;


msrp::Connection::Connection(Stack *stk)
  : mDescriptor(0), mStack(stk), mLocalAddress(0), mRemoteAddress(0), 
    mBufSz(0)
{
}


Connection::~Connection()
{
  delete mLocalAddress;
  delete mRemoteAddress;
}

  
void 
Connection::processReads()
{
   // see if there is any data to read from the network

   // pass it to the byte wrangler

   // see if the byte rangler has produced a Roar or some body Data 

   // pass the data to the corect OutgoingMessage 
}



bool
Connection::hasDataToWrite()
{

  return (mBufSz != 0)

}


bool
Connection::writeBuffer()
{

  int wrote;
  wrote = write(cursor, mBufSz);
  if (wrote == mBufSz) {
    mBufSz = 0;
    mCursor = mBuf;
    return true;
  } else {
    assert (wrote <= mBufSz);
    mBufSz -= wrote;
    mCursor += wrote;
    return false;
  }

}


int 
Connection::transmit(const MsrpRoar &roar, char *chunk,
		     int chunkLen, connFlag flag)
{

  // do something with the flag here
  
  if (hasDataToWrite) {
    writeBuffer();
    return 0;
  }
  
  if (mTID != roar.getTransationId()) {
    
    // generate tail, put into the buf
    // set new tid
    // 


  Data encoded;
  DataStream str(encoded);
  
  

  roar.encode(str);
  str.flush();
  
  int mBufSz = encoded.size();
  assert (mBufSz <= 4000);
  strcpy(mBuf, encoded.data(), mBufSz);
  cursor = mBuf;

  if (!writeBuffer()) {
    return 0;
  }

  int wrote;

  wrote = write(chunk, chunkLen);
  
  if (wrote != chunkLen) {
    
    
    return

  
  
  
    
