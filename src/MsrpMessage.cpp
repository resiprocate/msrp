#if defined(HAVE_CONFIG_H)
#include "common/config.hxx"
#endif

#include "common/Contents.hxx"
#include "common/OctetContents.hxx"
#include "common/HeaderFieldValueList.hxx"
#include "MsrpMessage.h"
#include "common/UnknownHeaderType.hxx"
#include "common/os/Coders.hxx"
#include "common/os/CountStream.hxx"
#include "common/os/Logger.hxx"
#include "common/os/MD5Stream.hxx"
#include "common/os/compat.hxx"
#include "common/os/vmd5.hxx"
#include "common/os/Coders.hxx"
#include "common/os/Random.hxx"
#include "common/os/ParseBuffer.hxx"
#include "common/MsgHeaderScanner.hxx"
#include "common/os/WinLeakCheck.hxx"

using namespace msrp;
using namespace std;

#define RESIPROCATE_SUBSYSTEM Subsystem::SIP

MsrpMessage::MsrpMessage(const Transport* fromWire)
   : mIsExternal(fromWire != 0),
     mTransport(fromWire),
     mStartLine(0),
     mContentsHfv(0),
     mContents(0),
     mRequest(false),
     mResponse(false),
     mCreatedTime(Timer::getTimeMicroSec()),
     mTlsDomain(Data::Empty)
{
   for (int i = 0; i < Headers::MAX_HEADERS; i++)
   {
      mHeaders[i] = 0;
   }
}

MsrpMessage::MsrpMessage(const MsrpMessage& from)
   : mStartLine(0),
     mContentsHfv(0),
     mContents(0),
     mCreatedTime(Timer::getTimeMicroSec())
{
   for (int i = 0; i < Headers::MAX_HEADERS; i++)
   {
      mHeaders[i] = 0;
   }

   *this = from;
}

Message*
MsrpMessage::clone() const
{
   return new MsrpMessage(*this);
}

MsrpMessage& 
MsrpMessage::operator=(const MsrpMessage& rhs)
{
   if (this != &rhs)
   {
      this->cleanUp();

      mIsExternal = rhs.mIsExternal;
      mTransport = rhs.mTransport;
      //mSource = rhs.mSource;
      //mDestination = rhs.mDestination;
      mStartLine = 0;
      mContentsHfv = 0;
      mContents = 0;
      mRequest = rhs.mRequest;
      mResponse = rhs.mResponse;
      mTlsDomain = rhs.mTlsDomain;
      
      for (int i = 0; i < Headers::MAX_HEADERS; i++)
      {
         if (rhs.mHeaders[i] != 0)
         {
            mHeaders[i] = new HeaderFieldValueList(*rhs.mHeaders[i]);
         }
         else
         {
            mHeaders[i] = 0;
         }
      }
  
      for (UnknownHeaders::const_iterator i = rhs.mUnknownHeaders.begin();
           i != rhs.mUnknownHeaders.end(); i++)
      {
         mUnknownHeaders.push_back(pair<Data, HeaderFieldValueList*>(
                                      i->first,
                                      new HeaderFieldValueList(*i->second)));
      }
      if (rhs.mStartLine != 0)
      {
         mStartLine = new HeaderFieldValueList(*rhs.mStartLine); 
      }
      if (rhs.mContents != 0)
      {
         mContents = rhs.mContents->clone();
      }
      else if (rhs.mContentsHfv != 0)
      {
         mContentsHfv = new HeaderFieldValue(*rhs.mContentsHfv);
      }
      else
      {
         // no body to copy
      }
   }

   return *this;
}

MsrpMessage::~MsrpMessage()
{
   cleanUp();
}

void
MsrpMessage::cleanUp()
{
   for (int i = 0; i < Headers::MAX_HEADERS; i++)
   {
      delete mHeaders[i];
      mHeaders[i] = 0;
   }

   for (UnknownHeaders::iterator i = mUnknownHeaders.begin();
        i != mUnknownHeaders.end(); i++)
   {
      delete i->second;
   }
   mUnknownHeaders.clear();
   
   for (vector<char*>::iterator i = mBufferList.begin();
        i != mBufferList.end(); i++)
   {
      delete [] *i;
   }
   mBufferList.clear();

   delete mStartLine;
   mStartLine = 0;
   delete mContents;
   mContents = 0;
   delete mContentsHfv;
   mContentsHfv = 0;
}

MsrpMessage*
MsrpMessage::make(const Data& data,  bool isExternal)
{
   Transport* external = (Transport*)(0xFFFF);
   MsrpMessage* msg = new MsrpMessage(isExternal ? external : 0);

   size_t len = data.size();
   char *buffer = new char[len + 5];

   msg->addBuffer(buffer);
   memcpy(buffer,data.data(), len);
   MsgHeaderScanner msgHeaderScanner;
   msgHeaderScanner.prepareForMessage(msg);
   
   char *unprocessedCharPtr;
   if (msgHeaderScanner.scanChunk(buffer, len, &unprocessedCharPtr) != MsgHeaderScanner::scrEnd)
   {
      DebugLog(<<"Scanner rejecting buffer as unparsable / fragmented.");
      DebugLog(<< data);
      delete msg; 
      msg = 0; 
      return 0;
   }

   // no pp error
   unsigned int used = unprocessedCharPtr - buffer;

   if (used < len)
   {
      // body is present .. add it up.
      // NB. The Sip Message uses an overlay (again)
      // for the body. It ALSO expects that the body
      // will be contiguous (of course).
      // it doesn't need a new buffer in UDP b/c there
      // will only be one datagram per buffer. (1:1 strict)

      msg->setBody(buffer+used,len-used);
      //DebugLog(<<"added " << len-used << " byte body");
   }

   return msg;
}

const Data& 
MsrpMessage::getTransactionId() const
{
   if (0 == mStartLine)
   {
      InfoLog( << "Missing start line " );
      throw Exception("Missing start line", __FILE__, __LINE__);
   }
   if (mRequest)
   {
      assert(!mResponse);
      return (dynamic_cast<ParserContainer<MsrpRequestLine>*>(mStartLine->getParserContainer())->front()).transactionId();
   }
   else
   {
      assert(!mRequest);
      return (dynamic_cast<ParserContainer<MsrpStatusLine>*>(mStartLine->getParserContainer())->front()).transactionId();
   }
}

bool
MsrpMessage::isRequest() const
{
   return mRequest;
}

bool
MsrpMessage::isResponse() const
{
   return mResponse;
}

Data
MsrpMessage::brief() const
{
   Data result(128, true);
#if 0
   static const Data  request("SipReq:  ");
   static const Data response("SipResp: ");
   static const Data tid(" tid=");
   static const Data contact(" contact=");
   static const Data cseq(" cseq=");
   static const Data slash(" / ");
   static const Data wire(" from(wire)");
   static const Data tu(" from(tu)");

   // !dlb! should be checked earlier 
#if 0
   if (!exists(h_CSeq))
   {
      result = "MALFORMED; missing CSeq";
      return result;
   }
   if (!exists(h_CallId))
   {
      result = "MALFORMED; missing Call-Id";
      return result;
   }
#endif
   
   if (isRequest()) 
   {
      result += request;
      MethodTypes meth = header(h_RequestLine).getMethod();
      if (meth != UNKNOWN)
      {
         result += getMethodName(meth);
      }
      else
      {
         result += header(h_RequestLine).unknownMethodName();
      }
      
      result += Symbols::SPACE;
      result += header(h_RequestLine).uri().getAor();
   }
   else if (isResponse())
   {
      result += response;
      result += Data(header(h_StatusLine).responseCode());
   }
   if (exists(h_Vias) && !this->header(h_Vias).empty())
   {
      result += tid;
      result += getTransactionId();
   }
   else
   {
      result += " NO-VIAS ";
   }

   result += cseq;
   if (header(h_CSeq).method() != UNKNOWN)
   {
      result += getMethodName(header(h_CSeq).method());
   }
   else
   {
      result += header(h_CSeq).unknownMethodName();
   }

   if (exists(h_Contacts) && !header(h_Contacts).empty())
   {
      result += contact;
      result += header(h_Contacts).front().uri().getAor();
   }
   
   result += slash;
   result += Data(header(h_CSeq).sequence());
   result += mIsExternal ? wire : tu;
#endif   
   return result;
}

bool
MsrpMessage::isClientTransaction() const
{
   assert(mRequest || mResponse);
   return ((mIsExternal && mResponse) || (!mIsExternal && mRequest));
}

// dynamic_cast &str to DataStream* to avoid CountStream?
std::ostream& 
MsrpMessage::encode(std::ostream& str) const
{
   if (mStartLine != 0)
   {
      mStartLine->encode(Data::Empty, str);
   }

   for (int i = 0; i < Headers::MAX_HEADERS; i++)
   {
      if (i != Headers::ContentLength) // !dlb! hack...
      {
         if (mHeaders[i] != 0)
         {
            mHeaders[i]->encode(i, str);
         }
      }
      else
      {
         if (mContents != 0)
         {
            size_t size;
            {
               CountStream cs(size);
               mContents->encode(cs);
            }
            str << "Content-Length: " << size << "\r\n";
         }
         else if (mContentsHfv != 0)
         {
            str << "Content-Length: " << mContentsHfv->mFieldLength << "\r\n";
         }
       }
   }

   for (UnknownHeaders::const_iterator i = mUnknownHeaders.begin(); 
        i != mUnknownHeaders.end(); i++)
   {
      i->second->encode(i->first, str);
   }

   str << Symbols::CRLF;
   
   if (mContents != 0)
   {
      mContents->encode(str);
   }
   else if (mContentsHfv != 0)
   {
      mContentsHfv->encode(str);
   }
   
   return str;
}

void
MsrpMessage::addBuffer(char* buf)
{
   mBufferList.push_back(buf);
}

void 
MsrpMessage::setStartLine(const char* st, int len)
{
   mStartLine = new HeaderFieldValueList;
   mStartLine->push_back(new HeaderFieldValue(st, len));
   ParseBuffer pb(st, len);
   const char* start;
   pb.skipWhitespace();
   pb.skipNonWhitespace();
   // !dys! do we want to save the transaction id here?
   pb.skipWhitespace();
   pb.skipNonWhitespace();
   start = pb.skipWhitespace();
   pb.skipNonWhitespace();
   MsrpMethodTypes method = getMsrpMethodType(start, pb.position() - start);
   if (method == OTHER) //probably a response...
   {
      if (int (len=pb.position()-start) == 3)
      {
         while (len--)
         {
            if (!isdigit(*start))
            {
               break;
            }
            start++;
         }

         if (len<0) mResponse = true;
                  
         if (mResponse)
         {
            mStartLine->setParserContainer(new ParserContainer<MsrpStatusLine>(mStartLine, Headers::NONE));
            //!dcm! should invoke the statusline parser here once it does
            //limited validation
         }
      }
   }
   if (!mResponse)
   {
      mStartLine->setParserContainer(new ParserContainer<MsrpRequestLine>(mStartLine, Headers::NONE));
      //!dcm! should invoke the responseline parser here once it does limited validation
      mRequest = true;
   }
}

void 
MsrpMessage::setBody(const char* start, int len)
{
   mContentsHfv = new HeaderFieldValue(start, len);
}

void
MsrpMessage::setContents(auto_ptr<Contents> contents)
{
   Contents* contentsP = contents.release();

   delete mContents;
   mContents = 0;
   delete mContentsHfv;
   mContentsHfv = 0;

   if (contentsP == 0)
   {
      // The semantics of setContents(0) are to delete message contents
      remove(h_ContentType);
      remove(h_ContentDisposition);
      remove(h_ContentTransferEncoding);
      remove(h_ContentLanguages);
      return;
   }

   mContents = contentsP;

   // copy contents headers into message
   if (mContents->exists(h_ContentDisposition))
   {
      header(h_ContentDisposition) = mContents->header(h_ContentDisposition);
   }
   if (mContents->exists(h_ContentTransferEncoding))
   {
      header(h_ContentTransferEncoding) = mContents->header(h_ContentTransferEncoding);
   }
   if (mContents->exists(h_ContentLanguages))
   {
      header(h_ContentLanguages) = mContents->header(h_ContentLanguages);
   }
   if (mContents->exists(h_ContentType))
   {
      header(h_ContentType) = mContents->header(h_ContentType);
      assert( header(h_ContentType).type() == mContents->getType().type() );
      assert( header(h_ContentType).subType() == mContents->getType().subType() );
   }
   else
   {
      header(h_ContentType) = mContents->getType();
   }
}

void 
MsrpMessage::setContents(const Contents* contents)
{ 
   if (contents)
   {
      setContents(auto_ptr<Contents>(contents->clone()));
   }
   else
   {
      setContents(auto_ptr<Contents>(0));
   }
}

Contents*
MsrpMessage::getContents() const
{
   if (mContents == 0 && mContentsHfv != 0)
   {
      if (!exists(h_ContentType))
      {
         StackLog(<< "MsrpMessage::getContents: ContentType header does not exist - implies no contents");
         return 0;
      }
      DebugLog(<< "MsrpMessage::getContents: " 
               << header(h_ContentType).type()
               << "/"
               << header(h_ContentType).subType());

      if ( Contents::getFactoryMap().find(header(h_ContentType)) == Contents::getFactoryMap().end() )
      {
         InfoLog(<< "MsrpMessage::getContents: got content type ("
                 << header(h_ContentType).type()
                 << "/"
                 << header(h_ContentType).subType()
                 << ") that is not known, "
                 << "returning as opaque application/octet-stream");
         mContents = Contents::getFactoryMap()[OctetContents::getStaticType()]->create(mContentsHfv, OctetContents::getStaticType());
      }
      else
      {
         mContents = Contents::getFactoryMap()[header(h_ContentType)]->create(mContentsHfv, header(h_ContentType));
      }
      assert( mContents );
      
      // copy contents headers into the contents
      if (exists(h_ContentDisposition))
      {
         mContents->header(h_ContentDisposition) = header(h_ContentDisposition);
      }
      if (exists(h_ContentTransferEncoding))
      {
         mContents->header(h_ContentTransferEncoding) = header(h_ContentTransferEncoding);
      }
      if (exists(h_ContentLanguages))
      {
         mContents->header(h_ContentLanguages) = header(h_ContentLanguages);
      }
      if (exists(h_ContentType))
      {
         mContents->header(h_ContentType) = header(h_ContentType);
      }
      // !dlb! Content-Transfer-Encoding?
   }
   return mContents;
}

auto_ptr<Contents>
MsrpMessage::releaseContents()
{
   auto_ptr<Contents> ret(getContents());
   if (ret.get() != 0)
   {
      // the buffer may go away...
      ret->checkParsed();
      mContents = 0;
      // ...here
   }
   setContents(auto_ptr<Contents>(0));
   
   return ret;
}

// unknown header interface
const StringCategories& 
MsrpMessage::header(const UnknownHeaderType& headerName) const
{
   for (UnknownHeaders::iterator i = mUnknownHeaders.begin();
        i != mUnknownHeaders.end(); i++)
   {
      // !dlb! case sensitive?
      if (i->first == headerName.getName())
      {
         HeaderFieldValueList* hfvs = i->second;
         if (hfvs->getParserContainer() == 0)
         {
            hfvs->setParserContainer(new ParserContainer<StringCategory>(hfvs, Headers::NONE));
         }
         return *dynamic_cast<ParserContainer<StringCategory>*>(hfvs->getParserContainer());
      }
   }
   // missing extension header
   assert(false);

   return *(StringCategories*)0;
}

StringCategories& 
MsrpMessage::header(const UnknownHeaderType& headerName)
{
   for (UnknownHeaders::iterator i = mUnknownHeaders.begin();
        i != mUnknownHeaders.end(); i++)
   {
      // !dlb! case sensitive?
      if (i->first == headerName.getName())
      {
         HeaderFieldValueList* hfvs = i->second;
         if (hfvs->getParserContainer() == 0)
         {
            hfvs->setParserContainer(new ParserContainer<StringCategory>(hfvs, Headers::NONE));
         }
         return *dynamic_cast<ParserContainer<StringCategory>*>(hfvs->getParserContainer());
      }
   }

   // create the list empty
   HeaderFieldValueList* hfvs = new HeaderFieldValueList;
   hfvs->setParserContainer(new ParserContainer<StringCategory>(hfvs, Headers::NONE));
   mUnknownHeaders.push_back(make_pair(headerName.getName(), hfvs));
   return *dynamic_cast<ParserContainer<StringCategory>*>(hfvs->getParserContainer());
}

bool
MsrpMessage::exists(const UnknownHeaderType& symbol) const
{
   for (UnknownHeaders::iterator i = mUnknownHeaders.begin();
        i != mUnknownHeaders.end(); i++)
   {
      if (i->first == symbol.getName())
      {
         return true;
      }
   }
   return false;
}

void
MsrpMessage::remove(const UnknownHeaderType& headerName)
{
   for (UnknownHeaders::iterator i = mUnknownHeaders.begin();
        i != mUnknownHeaders.end(); i++)
   {
      if (i->first == headerName.getName())
      {
         delete i->second;
         mUnknownHeaders.erase(i);
         return;
      }
   }
}

void
MsrpMessage::addHeader(Headers::Type header, const char* headerName, int headerLen, 
                      const char* start, int len)
{
   if (header != Headers::UNKNOWN)
   {
      if (mHeaders[header] == 0)
      {
         mHeaders[header] = new HeaderFieldValueList;
      }
      if (len)
      {
         mHeaders[header]->push_back(new HeaderFieldValue(start, len));
      }
   }
   else
   {
      for (UnknownHeaders::iterator i = mUnknownHeaders.begin();
           i != mUnknownHeaders.end(); i++)
      {
         if (strncasecmp(i->first.data(), headerName, headerLen) == 0)
         {
            // add to end of list
            if (len)
            {
               i->second->push_back(new HeaderFieldValue(start, len));
            }
            return;
         }
      }

      // didn't find it, add an entry
      HeaderFieldValueList *hfvs = new HeaderFieldValueList();
      if (len)
      {
         hfvs->push_back(new HeaderFieldValue(start, len));
      }
      mUnknownHeaders.push_back(pair<Data, HeaderFieldValueList*>(Data(headerName, headerLen),
                                                                  hfvs));
   }
}

Data&
MsrpMessage::getEncoded() 
{
   return mEncoded;
}

MsrpRequestLine& 
MsrpMessage::header(const RequestLineType& l)
{
   assert (!isResponse());
   if (mStartLine == 0 )
   { 
      mStartLine = new HeaderFieldValueList;
      mStartLine->push_back(new HeaderFieldValue);
      mStartLine->setParserContainer(new ParserContainer<MsrpRequestLine>(mStartLine, Headers::NONE));
      mRequest = true;
   }
   return dynamic_cast<ParserContainer<MsrpRequestLine>*>(mStartLine->getParserContainer())->front();
}

const MsrpRequestLine& 
MsrpMessage::header(const RequestLineType& l) const
{
   assert (!isResponse());
   if (mStartLine == 0 )
   { 
      // request line missing
      assert(false);
   }
   return dynamic_cast<ParserContainer<MsrpRequestLine>*>(mStartLine->getParserContainer())->front();
}

MsrpStatusLine& 
MsrpMessage::header(const StatusLineType& l)
{
   assert (!isRequest());
   if (mStartLine == 0 )
   { 
      mStartLine = new HeaderFieldValueList;
      mStartLine->push_back(new HeaderFieldValue);
      mStartLine->setParserContainer(new ParserContainer<MsrpStatusLine>(mStartLine, Headers::NONE));
      mResponse = true;
   }
   return dynamic_cast<ParserContainer<MsrpStatusLine>*>(mStartLine->getParserContainer())->front();
}

const MsrpStatusLine& 
MsrpMessage::header(const StatusLineType& l) const
{
   assert (!isRequest());
   if (mStartLine == 0 )
   { 
      // status line missing
      assert(false);
   }
   return dynamic_cast<ParserContainer<MsrpStatusLine>*>(mStartLine->getParserContainer())->front();
}

HeaderFieldValueList* 
MsrpMessage::ensureHeaders(Headers::Type type, bool single)
{
   HeaderFieldValueList* hfvs = mHeaders[type];
   
   // empty?
   if (hfvs == 0)
   {
      // create the list with a new component
      hfvs = new HeaderFieldValueList;
      mHeaders[type] = hfvs;
      if (single)
      {
         HeaderFieldValue* hfv = new HeaderFieldValue;
         hfvs->push_back(hfv);
      }
   }
   // !dlb! not thrilled about checking this every access
   else if (single)
   {
      if (hfvs->parsedEmpty())
      {
         // create an unparsed shared header field value // !dlb! when will this happen?
         hfvs->push_back(new HeaderFieldValue(Data::Empty.data(), 0));
      }
   }

   return hfvs;
}

HeaderFieldValueList* 
MsrpMessage::ensureHeaders(Headers::Type type, bool single) const
{
   HeaderFieldValueList* hfvs = mHeaders[type];
   
   // empty?
   if (hfvs == 0)
   {
      // header missing
      // assert(false);
      InfoLog( << "Missing Header [" << Headers::getHeaderName(type) << "]");      
      DebugLog (<< *this);
      throw Exception("Missing header " + Headers::getHeaderName(type), __FILE__, __LINE__);
   }
   // !dlb! not thrilled about checking this every access
   else if (single)
   {
      if (hfvs->parsedEmpty())
      {
         // !dlb! when will this happen?
         // assert(false);
         InfoLog( << "Missing Header " << Headers::getHeaderName(type) );
         DebugLog (<< *this);
         throw Exception("Empty header", __FILE__, __LINE__);
      }
   }

   return hfvs;
}

// type safe header accessors
bool    
MsrpMessage::exists(const HeaderBase& headerType) const 
{
   return mHeaders[headerType.getTypeNum()] != 0;
};

void
MsrpMessage::remove(const HeaderBase& headerType)
{
   delete mHeaders[headerType.getTypeNum()]; 
   mHeaders[headerType.getTypeNum()] = 0; 
};

#ifndef PARTIAL_TEMPLATE_SPECIALIZATION

#undef defineHeader
#define defineHeader(_header)                                                                           \
const H_##_header::Type&                                                                                \
MsrpMessage::header(const H_##_header& headerType) const                                                 \
{                                                                                                       \
   HeaderFieldValueList* hfvs = ensureHeaders(headerType.getTypeNum(), true);                           \
   if (hfvs->getParserContainer() == 0)                                                                 \
   {                                                                                                    \
      hfvs->setParserContainer(new ParserContainer<H_##_header::Type>(hfvs, headerType.getTypeNum()));  \
   }                                                                                                    \
   return dynamic_cast<ParserContainer<H_##_header::Type>*>(hfvs->getParserContainer())->front();       \
}                                                                                                       \
                                                                                                        \
H_##_header::Type&                                                                                      \
MsrpMessage::header(const H_##_header& headerType)                                                       \
{                                                                                                       \
   HeaderFieldValueList* hfvs = ensureHeaders(headerType.getTypeNum(), true);                           \
   if (hfvs->getParserContainer() == 0)                                                                 \
   {                                                                                                    \
      hfvs->setParserContainer(new ParserContainer<H_##_header::Type>(hfvs, headerType.getTypeNum()));  \
   }                                                                                                    \
   return dynamic_cast<ParserContainer<H_##_header::Type>*>(hfvs->getParserContainer())->front();       \
}

#undef defineMultiHeader
#define defineMultiHeader(_header)                                                              \
const H_##_header##s::Type&                                                                     \
MsrpMessage::header(const H_##_header##s& headerType) const                                      \
{                                                                                               \
   HeaderFieldValueList* hfvs = ensureHeaders(headerType.getTypeNum(), false);                  \
   if (hfvs->getParserContainer() == 0)                                                         \
   {                                                                                            \
      hfvs->setParserContainer(new H_##_header##s::Type(hfvs, headerType.getTypeNum()));        \
   }                                                                                            \
   return *dynamic_cast<H_##_header##s::Type*>(hfvs->getParserContainer());                     \
}                                                                                               \
                                                                                                \
H_##_header##s::Type&                                                                           \
MsrpMessage::header(const H_##_header##s& headerType)                                            \
{                                                                                               \
   HeaderFieldValueList* hfvs = ensureHeaders(headerType.getTypeNum(), false);                  \
   if (hfvs->getParserContainer() == 0)                                                         \
   {                                                                                            \
      hfvs->setParserContainer(new H_##_header##s::Type(hfvs, headerType.getTypeNum()));        \
   }                                                                                            \
   return *dynamic_cast<H_##_header##s::Type*>(hfvs->getParserContainer());                     \
}

defineHeader(ContentDisposition);
defineHeader(ContentId);
defineHeader(ContentTransferEncoding);
defineHeader(ContentEncoding);
defineHeader(ContentLength);
defineHeader(ContentType);
defineHeader(ToPath);
defineHeader(FromPath);
defineHeader(MessageId);
defineHeader(ReportSuccess);
defineHeader(ReportFailure);
defineHeader(ByteRange);
defineHeader(Status);
defineHeader(MIMEVersion);
defineMultiHeader(ContentLanguage);

#endif

const HeaderFieldValueList*
MsrpMessage::getRawHeader(Headers::Type headerType) const
{
   return mHeaders[headerType];
}

void
MsrpMessage::setRawHeader(const HeaderFieldValueList* hfvs, Headers::Type headerType)
{
   if (mHeaders[headerType] != hfvs)
   {
      delete mHeaders[headerType];
      mHeaders[headerType] = new HeaderFieldValueList(*hfvs);
   }
}

#if defined(DEBUG) && defined(DEBUG_MEMORY)
namespace msrp
{

void*
operator new(size_t size)
{
   void * p = std::operator new(size);
   DebugLog(<<"operator new | " << hex << p << " | "
            << dec << size);
   if (size == 60)
   {
      3;
   }
   
   return p;
}

void operator delete(void* p)
{
   DebugLog(<<"operator delete | " << hex << p << dec);
   return std::operator delete( p );
}

void operator delete[](void* p)
{
   DebugLog(<<"operator delete [] | " << hex << p << dec);
   return std::operator delete[] ( p );
}
 
}

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
