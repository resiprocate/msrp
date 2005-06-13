#if defined(HAVE_CONFIG_H)
#include "msrprocate/config.hxx"
#endif

#include "msrprocate/os/Logger.hxx"
#include "msrprocate/ByteWrangler.hxx"
#include "msrprocate/SipMessage.hxx"
#include "msrprocate/Security.hxx"
#include "msrprocate/os/WinLeakCheck.hxx"

using namespace msrp;

#define MSRPROCATE_SUBSYSTEM Subsystem::TRANSPORT

char 
ByteWrangler::connectionStates[ByteWrangler::MAX][32] = { "NewMessage", "ReadingHeaders", "PartialBody" };


ByteWrangler::ByteWrangler()
   : mSendPos(0),
     mWho(),
     mMessage(0),
     mBuffer(0),
     mBufferPos(0),
     mBufferSize(0),
     mLastUsed(0),
     mState(NewMessage)
{
   DebugLog (<< "ByteWrangler::ByteWrangler, no params: " << this);
}

ByteWrangler::ByteWrangler(const Tuple& who)
   : mSendPos(0),
     mWho(who),
     mMessage(0),
     mBuffer(0),
     mBufferPos(0),
     mBufferSize(0),
     mLastUsed(Timer::getTimeMs()),
     mState(NewMessage)
{
   DebugLog (<< "ByteWrangler::ByteWrangler, who: " << mWho << " " << this);
}

ByteWrangler::~ByteWrangler()
{
   while (!mOutstandingSends.empty())
   {
      SendData* sendData = mOutstandingSends.front();
      mWho.transport->fail(sendData->transactionId);
      delete sendData;
      mOutstandingSends.pop_front();
   }
//   delete mBuffer;
//   delete mMessage;
}

ConnectionId
ByteWrangler::getId() const
{
   return mWho.connectionId;
}

void
ByteWrangler::preparseNewBytes(int bytesRead, Fifo<TransactionMessage>& fifo)
{
   assert(mWho.transport);

   DebugLog(<< "In State: " << connectionStates[mState]);
   //getConnectionManager().touch(this); -- !dcm!
   
  start:   // If there is an overhang come back here, effectively recursing
   
   switch(mState)
   {
      case NewMessage:
      {
         if (strncmp(mBuffer + mBufferPos, Symbols::CRLFCRLF, 4) == 0)
         {
            StackLog(<<"Throwing away incoming firewall keep-alive");
            mBufferPos += 4;
            bytesRead -= 4;
            if (bytesRead)
            {
               goto start;
            }
            else
            {
               return;
            }
         }
         assert(mWho.transport);
         mMessage = new SipMessage(mWho.transport);
         
         DebugLog(<< "ByteWrangler::process setting source " << mWho);
         mMessage->setSource(mWho);
         mMessage->setTlsDomain(mWho.transport->tlsDomain());
         mMsgHeaderScanner.prepareForMessage(mMessage);
         // Fall through to the next case.
      }
      case ReadingHeaders:
      {
         unsigned int chunkLength = mBufferPos + bytesRead;
         char *unprocessedCharPtr;
         MsgHeaderScanner::ScanChunkResult scanChunkResult =
            mMsgHeaderScanner.scanChunk(mBuffer,
                                        chunkLength,
                                        &unprocessedCharPtr);
         if (scanChunkResult == MsgHeaderScanner::scrError)
         {
            //.jacob. Not a terribly informative warning.
            WarningLog(<< "Discarding preparse!");
            delete mBuffer;
            mBuffer = 0;
            delete mMessage;
            mMessage = 0;
            //.jacob. Shouldn't the state also be set here?
            delete this;
            return;
         }
         mMessage->addBuffer(mBuffer);
         unsigned int numUnprocessedChars =
            (mBuffer + chunkLength) - unprocessedCharPtr;
         if (scanChunkResult == MsgHeaderScanner::scrNextChunk)
         {
            // Message header is incomplete...
            if (numUnprocessedChars == 0)
            {
               // ...but the chunk is completely processed.
               //.jacob. I've discarded the "assigned" concept.
               //DebugLog(<< "Data assigned, not fragmented, not complete");
               mBuffer = new char[ChunkSize + MsgHeaderScanner::MaxNumCharsChunkOverflow];
               mBufferPos = 0;
               mBufferSize = ChunkSize;
            }
            else
            {
               // ...but some of the chunk must be shifted into the next one.
               size_t size = numUnprocessedChars*3/2;
               if ( size < ByteWrangler::ChunkSize )
               {
                  size = ByteWrangler::ChunkSize;
               }
               char* newBuffer = new char[size + MsgHeaderScanner::MaxNumCharsChunkOverflow];
               memcpy(newBuffer, unprocessedCharPtr, numUnprocessedChars);
               mBuffer = newBuffer;
               mBufferPos = numUnprocessedChars;
               mBufferSize = size;
            }
            mState = ReadingHeaders;
         }
         else
         {         
            // The message header is complete.
            size_t contentLength = mMessage->header(h_ContentLength).value();
            
            if (numUnprocessedChars < contentLength)
            {
               // The message body is incomplete.
               char* newBuffer = new char[contentLength + MsgHeaderScanner::MaxNumCharsChunkOverflow];
               memcpy(newBuffer, unprocessedCharPtr, numUnprocessedChars);
               mBufferPos = numUnprocessedChars;
               mBufferSize = contentLength;
               mBuffer = newBuffer;
            
               mState = PartialBody;
            }
            else
            {
               // The message body is complete.
               mMessage->setBody(unprocessedCharPtr, contentLength);
               if (!transport()->basicCheck(*mMessage))
               {
                  delete mMessage;
                  mMessage = 0;
               }
               else
               {
                  Transport::stampReceived(mMessage);
                  DebugLog(<< "##Connection: " << *this << " received: " << *mMessage);
                  fifo.add(mMessage);
                  mMessage = 0;                  
               }

               int overHang = numUnprocessedChars - contentLength;

               mState = NewMessage;
               mBuffer = 0;               
               if (overHang > 0) 
               {
                  // The next message has been partially read.
                  size_t size = overHang*3/2;
                  if ( size < ByteWrangler::ChunkSize )
                  {
                     size = ByteWrangler::ChunkSize;
                  }
                  char* newBuffer = new char[size + MsgHeaderScanner::MaxNumCharsChunkOverflow];
                  memcpy(newBuffer,
                         unprocessedCharPtr + contentLength,
                         overHang);
                  mBuffer = newBuffer;
                  mBufferPos = 0;
                  mBufferSize = size;
                  
                  DebugLog (<< "Extra bytes after message: " << overHang);
                  DebugLog (<< Data(mBuffer, overHang));
                  
                  bytesRead = overHang;
                  goto start;
               }
            }
         }
         break;
      }
      case PartialBody:
      {
         size_t contentLength = mMessage->header(h_ContentLength).value();
         mBufferPos += bytesRead;
         if (mBufferPos == contentLength)
         {
            mMessage->setBody(mBuffer, contentLength);
            if (!transport()->basicCheck(*mMessage))
            {
               delete mMessage;
               mMessage = 0;
            }
            else
            {
               DebugLog(<< "##Connection: " << *this << " received: " << *mMessage);

               Transport::stampReceived(mMessage);
               fifo.add(mMessage);
            }
            mState = NewMessage;
            mBuffer = 0;            
         }
         break;
      }
      default:
         assert(0);
   }
}
            
std::pair<char*, size_t> 
ByteWrangler::getWriteBuffer()
{
   if (mState == NewMessage)
   {
      DebugLog (<< "Creating buffer for " << *this);

      mBuffer = new char [ByteWrangler::ChunkSize + MsgHeaderScanner::MaxNumCharsChunkOverflow];
      mBufferSize = ByteWrangler::ChunkSize;
      mBufferPos = 0;
   }
   return std::make_pair(mBuffer + mBufferPos, mBufferSize - mBufferPos);
}
            
void 
ByteWrangler::setBuffer(char* bytes, int count)
{
   mBuffer = bytes;
   mBufferPos = 0;
   mBufferSize = count;
}

Transport* 
ByteWrangler::transport()
{
   assert(this);
   return mWho.transport;
}

std::ostream& 

msrp::operator<<(std::ostream& strm, const msrp::ByteWrangler& c)

{

   strm << "CONN_BASE: " << &c << " " << c.mWho;

   return strm;

}




/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */

