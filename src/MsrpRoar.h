#if !defined(MSRP_MSRPMESSAGE_HXX)
#define MSRP_MSRPMESSAGE_HXX 

#include <sys/types.h>

#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#endif

#include <list>
#include <vector>
#include <utility>
#include <memory> 

#include "common/Contents.hxx"
#include "common/Headers.hxx"
#include "common/TransactionMessage.hxx"
#include "common/ParserContainer.hxx"
#include "common/ParserCategories.hxx"
//#include "common/Transport.hxx"
#include "common/os/BaseException.hxx"
#include "common/os/Data.hxx"
#include "common/os/Socket.hxx"
#include "common/os/Timer.hxx"
#include "common/os/HeapInstanceCounter.hxx"

namespace msrp
{
class Transport;

class Contents;
class UnknownHeaderType;

class MsrpRoar : public TransactionMessage
{
   public:
      RESIP_HeapCount(MsrpRoar);
      typedef std::list< std::pair<Data, HeaderFieldValueList*> > UnknownHeaders;

      explicit MsrpRoar(const Transport* fromWire = 0);
      // .dlb. public, allows pass by value to compile.
      MsrpRoar(const MsrpRoar& message);

      // .dlb. sure would be nice to have overloaded return value here..
      virtual Roar* clone() const;

      MsrpRoar& operator=(const MsrpRoar& rhs);
      
      
      // returns the transaction id.
      virtual const Data& getTransactionId() const;

      virtual ~MsrpRoar();

      static MsrpRoar* make(const Data& buffer, bool isExternal = false);

      class Exception : public BaseException
      {
         public:
            Exception(const Data& msg, const Data& file, const int line)
               : BaseException(msg, file, line) {}

            const char* name() const { return "MsrpRoar::Exception"; }
      };

      void setFromTU() 
      {
         mIsExternal = false;
      }

      void setFromExternal()
      {
         mIsExternal = true;
      }
      
      bool isExternal() const
      {
         return mIsExternal;
      }

      virtual bool isClientTransaction() const;
      
      virtual std::ostream& encode(std::ostream& str) const;
      
      Data brief() const;

      bool isRequest() const;
      bool isResponse() const;

      const MsrpRequestLine& 
      header(const RequestLineType& l) const;

      MsrpRequestLine& 
      header(const RequestLineType& l);

      const MsrpStatusLine& 
      header(const StatusLineType& l) const;

      MsrpStatusLine& 
      header(const StatusLineType& l);

      bool exists(const HeaderBase& headerType) const;
      void remove(const HeaderBase& headerType);


#define defineHeader(_header)                                                   \
      const H_##_header::Type& header(const H_##_header& headerType) const;     \
      H_##_header::Type& header(const H_##_header& headerType)

#define defineMultiHeader(_header)                                                      \
      const H_##_header##s::Type& header(const H_##_header##s& headerType) const;       \
      H_##_header##s::Type& header(const H_##_header##s& headerType)

      defineHeader(ContentDisposition);
      defineHeader(ContentId);
      defineHeader(ContentTransferEncoding);
      defineHeader(ContentEncoding);
      defineHeader(ContentLength);
      defineHeader(ContentType);
      defineHeader(ToPath);
      defineHeader(FromPath);
      defineHeader(RoarId);
      defineHeader(ReportSuccess);
      defineHeader(ReportFailure);
      defineHeader(ByteRange);
      defineHeader(Status);
      defineHeader(MIMEVersion);
      defineMultiHeader(ContentLanguage);

      // unknown header interface
      const StringCategories& header(const UnknownHeaderType& symbol) const;
      StringCategories& header(const UnknownHeaderType& symbol);
      bool exists(const UnknownHeaderType& symbol) const;
      void remove(const UnknownHeaderType& symbol);

      // typeless header interface
      const HeaderFieldValueList* getRawHeader(Headers::Type headerType) const;
      void setRawHeader(const HeaderFieldValueList* hfvs, Headers::Type headerType);
      const UnknownHeaders& getRawUnknownHeaders() const {return mUnknownHeaders;}

      Contents* getContents() const;
      // removes the contents from the message
      std::auto_ptr<Contents> releaseContents();

      void setContents(const Contents* contents);
      void setContents(std::auto_ptr<Contents> contents);

      // transport interface
      void setStartLine(const char* start, int len); 

      void setBody(const char* start, int len); 
      
      // add HeaderFieldValue given enum, header name, pointer start, content length
      void addHeader(Headers::Type header,
                     const char* headerName, int headerLen, 
                     const char* start, int len);

      // Interface used to determine which Transport was used to receive a
      // particular MsrpRoar. If the MsrpRoar was not received from the
      // wire, getReceivedTransport() returns 0. Set in constructor
      const Transport* getReceivedTransport() const { return mTransport; }

      // Returns the source tuple that the message was received from
      // only makes sense for messages received from the wire
      //void setSource(const Tuple& tuple) { mSource = tuple; }   // !dys! commented out for now
      //const Tuple& getSource() const { return mSource; }  // !dys! commented out for now 
      
      // Used by the stateless interface to specify where to send a request/response
      //void setDestination(const Tuple& tuple) { mDestination = tuple; }  // !dys! commented out for now
      //Tuple& getDestination() { return mDestination; } // !dys! commented out for now

      void addBuffer(char* buf);

      // returns the encoded buffer which was encoded by resolve()
      // should only be called by the TransportSelector
      Data& getEncoded();

      UInt64 getCreatedTimeMicroSec() {return mCreatedTime;}

      const Data& getTlsDomain() const { return mTlsDomain; }
      void setTlsDomain(const Data& domain) { mTlsDomain = domain; }

   protected:
      void cleanUp();
   
   private:
      std::ostream& 
      encode(std::ostream& str, bool isSipFrag) const;      

      void copyFrom(const MsrpRoar& message);

      HeaderFieldValueList* ensureHeaders(Headers::Type type, bool single);
      HeaderFieldValueList* ensureHeaders(Headers::Type type, bool single) const; // throws if not present

      // indicates this message came from the wire, set by the Transport
      bool mIsExternal;
      
      // raw text corresponding to each typed header (not yet parsed)
      mutable HeaderFieldValueList* mHeaders[Headers::MAX_HEADERS];

      // raw text corresponding to each unknown header
      mutable UnknownHeaders mUnknownHeaders;
  
      // !jf!
      const Transport* mTransport;

      // For messages received from the wire, this indicates where it came
      // from. Can be used to get to the Transport and/or reliable Connection
      // Tuple mSource; // !dys! commented out for now

      // Used by the TU to specify where a message is to go
      // Tuple mDestination;  // !dys! commented out for now
      
      // Raw buffers coming from the Transport. message manages the memory
      std::vector<char*> mBufferList;

      // special case for the first line of message
      mutable HeaderFieldValueList* mStartLine;

      // raw text for the contents (all of them)
      mutable HeaderFieldValue* mContentsHfv;

      // lazy parser for the contents
      mutable Contents* mContents;

      // is a request or response
      mutable bool mRequest;
      mutable bool mResponse;

      Data mEncoded; // to be retransmitted
      UInt64 mCreatedTime;

      // domain associated with this message for tls cert
      Data mTlsDomain;

      friend class TransportSelector;
};

}

#undef ensureHeaderTypeUseable
#undef ensureSingleHeader
#undef ensureMultiHeader
#undef defineHeader
#undef defineMultiHeader

#endif

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
