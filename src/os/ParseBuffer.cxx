#include <cassert>

#include "src/os/Logger.hxx"
#include "src/os/ParseBuffer.hxx"
#include "src/os/DataStream.hxx"

using namespace msrp;

#define RESIPROCATE_SUBSYSTEM Subsystem::SIP

const char* ParseBuffer::ParamTerm = ";?"; // maybe include "@>,"?
const char* ParseBuffer::Whitespace = " \t\r\n";
const Data ParseBuffer::Pointer::msg("dereferenced ParseBuffer eof");

ParseBuffer::ParseBuffer(const char* buff, unsigned int len, 
                         const Data& errorContext)
   : mBuff(buff),
     mPosition(buff),
     mEnd(buff+len),
     mErrorContext(errorContext)
{}

ParseBuffer::ParseBuffer(const Data& data,
            const Data& errorContext)
   : mBuff(data.data()),
     mPosition(mBuff),
     mEnd(mBuff + data.size()),
     mErrorContext(errorContext)
{}

ParseBuffer::ParseBuffer(const ParseBuffer& rhs)
   : mBuff(rhs.mBuff),
     mPosition(rhs.mPosition),
     mEnd(rhs.mEnd),
     mErrorContext(rhs.mErrorContext)
{}

ParseBuffer& 
ParseBuffer::operator=(const ParseBuffer& rhs)
{
   mBuff = rhs.mBuff;
   mPosition = rhs.mPosition;
   mEnd = rhs.mEnd;

   return *this;
}

const Data& 
ParseBuffer::getContext() const
{
   return mErrorContext;
}

void
ParseBuffer::reset(const char* pos)
{
   assert( mBuff <= mEnd);
   assert( (pos >= mBuff) && (pos <= mEnd) );
   mPosition = pos;
}

ParseBuffer::Pointer 
ParseBuffer::skipChar()
{
   if (eof())
   {
      fail(__FILE__, __LINE__,"skipped over eof");
   }
   return Pointer(*this, ++mPosition, eof());
}

ParseBuffer::Pointer
ParseBuffer::skipChar(char c)
{
   if (eof())
   {
      fail(__FILE__, __LINE__,"skipped over eof");
   }
   if (*mPosition != c)
   {
      Data msg("expected '");
      msg += c;
      msg += "'";
      fail(__FILE__, __LINE__,msg);
   }
   return Pointer(*this, ++mPosition, eof());
}

ParseBuffer::Pointer 
ParseBuffer::skipChars(const char* cs)
{
   const char* match = cs;
   while (*match != 0)
   {
      if (eof() || (*match != *mPosition))
      {
          Data msg("Expected \"");
          msg += cs;
          msg +=  "\"";
         fail(__FILE__, __LINE__,msg);
      }
      match++;
      mPosition++;
   }
   return Pointer(*this, mPosition, eof());
}

ParseBuffer::Pointer 
ParseBuffer::skipChars(const Data& cs)
{
   const char* match = cs.data();
   for(Data::size_type i = 0; i < cs.size(); i++)
   {
      if (eof() || (*match != *mPosition))
      {
          Data msg( "Expected \"");
          msg += cs;
          msg += "\"";
         fail(__FILE__, __LINE__,msg);
      }
      match++;
      mPosition++;
   }
   return Pointer(*this, mPosition, eof());
}

ParseBuffer::Pointer 
ParseBuffer::skipNonWhitespace()
{
   while (mPosition < mEnd)
   {
      switch (*mPosition)
      {
         case ' ' :
         case '\t' : 
         case '\r' : 
         case '\n' : 
            return Pointer(*this, mPosition, false);
         default : 
            mPosition++;
      }
   }
   return Pointer(*this, mPosition, eof());
}

ParseBuffer::Pointer 
ParseBuffer::skipWhitespace()
{
   while (mPosition < mEnd)
   {
      switch (*mPosition)
      {
         case ' ' :
         case '\t' : 
         case '\r' : 
         case '\n' : 
         {
            mPosition++;
            break;
         }
         default : 
            return Pointer(*this, mPosition, false);
      }
   }
   return Pointer(*this, mPosition, true);
}

// "SIP header field values can be folded onto multiple lines if the
//  continuation line begins with a space or horizontal tab"

// CR can be quote with \ within "" and comments -- treat \CR as whitespace
ParseBuffer::Pointer 
ParseBuffer::skipLWS()
{
   enum State {WS, CR, LF};
   State state = WS;
   while (mPosition < mEnd)
   {
      char c = *mPosition++;
      if (c == '\\')
      {
         // treat escaped CR and LF as space
         c = *mPosition++;
         if (c == '\r' || c == '\n')
         {
            c = ' ';
         }
      }
      switch (*mPosition++)
      {
         case ' ' :
         case '\t' : 
         {
            state = WS;
            break;
         }
         case '\r' : 
         {
            state = CR;
            break;
         }
         case '\n' : 
         {
            if (state == CR)
            {
               state = LF;
            }
            else
            {
               state = WS;
            }
            break;
         }
         default : 
         {
            // terminating CRLF not skipped
            if (state == LF)
            {
               mPosition -= 3;
            }
            else
            {
               mPosition--;
            }
            return Pointer(*this, mPosition, false);
         }
      }
   }
   return Pointer(*this, mPosition, true);
}

ParseBuffer::Pointer
ParseBuffer::skipToTermCRLF()
{
   while (mPosition < mEnd)
   {
      static Data CRLF("\r\n");
      skipToChars(CRLF);
      mPosition += 2;
      if ((*mPosition != ' ' &&
           *mPosition != '\t' &&
           // check for \CRLF -- not terminating
           //           \\CRLF -- terminating
           ((mPosition-3 < mBuff || *(mPosition-3) != '\\') ||
            (mPosition-4 > mBuff && *(mPosition-4) == '\\'))))
      {
         mPosition -= 2;
         return Pointer(*this, mPosition, false);
      }
   }
   return Pointer(*this, mPosition, true);
}

ParseBuffer::Pointer 
ParseBuffer::skipToChar(char c)
{
   while (mPosition < mEnd)
   {
      if (*mPosition == c)
      {
         return Pointer(*this, mPosition, false);
      }
      else
      {
         mPosition++;
      }
   }
   return Pointer(*this, mEnd, true);
}

ParseBuffer::Pointer
ParseBuffer::skipToChars(const char* cs)
{
   assert(cs);
   unsigned int l = strlen(cs);

   const char* rpos;
   const char* cpos;
   while (mPosition < mEnd)
   {
      rpos = mPosition;
      cpos = cs;
      for (unsigned int i = 0; i < l; i++)
      {
         if (*cpos++ != *rpos++)
         {
            mPosition++;
            goto skip;
         }
      }
      return Pointer(*this, mPosition, false);
     skip: ;
   }
   return Pointer(*this, mPosition, true);
}

ParseBuffer::Pointer
ParseBuffer::skipToChars(const Data& cs)
{
   const unsigned int l = static_cast<unsigned int>(cs.size());

   const char* rpos;
   const char* cpos;
   while (mPosition < mEnd)
   {
      cpos = cs.data();
      rpos = mPosition;
      for (size_t i = 0; i < l; i++)
      {
         if (*cpos++ != *rpos++)
         {
            mPosition++;
            goto skip;
         }
      }
      return Pointer(*this, mPosition, false);
     skip: ;
   }
   return Pointer(*this, mPosition, true);
}

bool oneOf(char c, const char* cs)
{
   while (*cs)
   {
      if (c == *(cs++))
      {
         return true;
      }
   }
   return false;
}

bool oneOf(char c, const Data& cs)
{
   for (Data::size_type i = 0; i < cs.size(); i++)
   {
      if (c == cs[i])
      {
         return true;
      }
   }
   return false;
}

ParseBuffer::Pointer 
ParseBuffer::skipToOneOf(const char* cs)
{
   while (mPosition < mEnd)
   {
      if (oneOf(*mPosition, cs))
      {
         return Pointer(*this, mPosition, false);
      }
      else
      {
         mPosition++;
      }
   }
   return Pointer(*this, mPosition, true);
}

ParseBuffer::Pointer 
ParseBuffer::skipToOneOf(const char* cs1,
                         const char* cs2)
{
   while (mPosition < mEnd)
   {
      if (oneOf(*mPosition, cs1) ||
          oneOf(*mPosition, cs2))
      {
         return Pointer(*this, mPosition, false);
      }
      else
      {
         mPosition++;
      }
   }
   return Pointer(*this, mPosition, true);
}

ParseBuffer::Pointer 
ParseBuffer::skipToOneOf(const Data& cs)
{
   while (mPosition < mEnd)
   {
      if (oneOf(*mPosition, cs))
      {
         return Pointer(*this, mPosition, false);
      }
      else
      {
         mPosition++;
      }
   }
   return Pointer(*this, mPosition, true);
}

ParseBuffer::Pointer 
ParseBuffer::skipToOneOf(const Data& cs1,
                         const Data& cs2)
{
   while (mPosition < mEnd)
   {
      if (oneOf(*mPosition, cs1) ||
          oneOf(*mPosition, cs2))
      {
         return Pointer(*this, mPosition, false);
      }
      else
      {
         mPosition++;
      }
   }
   return Pointer(*this, mPosition, true);
}

const char*
ParseBuffer::skipToEndQuote(char quote)
{
   while (mPosition < mEnd)
   {
      // !dlb! mark character encoding
      if (*mPosition == '\\')
      {
         mPosition += 2;
      }
      else if (*mPosition == quote)
      {
         return mPosition;
      }
      else
      {
         mPosition++;
      }
   }

   {
      Data msg("Missing '");
      msg += quote;
      msg += "'";
      fail(__FILE__,__LINE__,msg);
   }
   return 0;
}

ParseBuffer::Pointer 
ParseBuffer::skipN(int count)
{
   mPosition += count;
   if (mPosition > mEnd)
   {

      fail(__FILE__, __LINE__, "skipped eof");
   }
   return Pointer(*this, mPosition, eof());
}

ParseBuffer::Pointer 
ParseBuffer::skipToEnd()
{
   mPosition = mEnd;
   return Pointer(*this, mPosition, true);
}

const char*
ParseBuffer::skipBackChar()
{
   if (bof())
   {
      fail(__FILE__, __LINE__,"backed over beginning of buffer");
   }
   mPosition--;
   return mPosition;
}

const char*
ParseBuffer::skipBackN(int count)
{
   mPosition -= count;
   if (bof())
   { 
     fail(__FILE__, __LINE__,"backed over beginning of buffer");
   }
   return mPosition;
}

// abcde
//     ^
// skipBackChar('d');
// abcde
//    ^
// skipChar('d');
// abcde
//     ^
const char*
ParseBuffer::skipBackChar(char c)
{
   if (bof())
   {
      fail(__FILE__, __LINE__,"backed over beginning of buffer");
   }
   if (*(--mPosition) != c)
   {
       Data msg( "Expected '");
       msg += c;
       msg += "'";
      fail(__FILE__, __LINE__,msg);
   }
   return mPosition;
}

// abcde
//      ^
// skipBackToChar('c');
// abcde
//   ^
const char*
ParseBuffer::skipBackToChar(char c)
{
   while (!bof())
   {
      if (*(--mPosition) == c)
      {
         return ++mPosition;
      }
   }
   return mBuff;
}

void
ParseBuffer::data(Data& data, const char* start) const
{
   if (!(mBuff <= start && start <= mPosition))
   {
      fail(__FILE__, __LINE__,"Bad anchor position");
   }

   if (data.mMine == Data::Take)
   {
      delete[] data.mBuf;
   }
   data.mSize = (unsigned int)(mPosition - start);
   data.mBuf = const_cast<char*>(start);
   data.mCapacity = data.mSize;
   data.mMine = Data::Share;
}

Data
ParseBuffer::data(const char* start) const
{
   if (!(mBuff <= start && start <= mPosition))
   {
      
      fail(__FILE__, __LINE__,"Bad anchor position");
   }

   Data data(start, mPosition - start);
   return data;
}

int
ParseBuffer::integer()
{
   if (this->eof())
   {
      fail(__FILE__, __LINE__,"Expected a digit, got eof ");
   }

   char c = *position();

   int signum = 1;
   if (c == '-')
   {
      signum = -1;
      skipChar();
      c = *position();
   }
   else if (c == '+')
   {
      skipChar();
      c = *position();
   }

   if (!isdigit(c))
   {
       Data msg("Expected a digit, got: ");
       msg += Data(mPosition, (mEnd - mPosition));
      fail(__FILE__, __LINE__,msg);
   }
   
   int num = 0;
   while (!eof() && isdigit(*mPosition))
   {
      num = num*10 + (*mPosition-'0');
      skipChar();
   }
   
   return signum*num;
}

//!dcm! -- merge these, ask about length checks
unsigned long
ParseBuffer::unsignedInteger()
{
   if (this->eof())
   {

      fail(__FILE__, __LINE__,"Expected a digit, got eof ");
   }

   const char* p = mPosition;
   assert(p);
   char c = *p;

   if (!isdigit(c))
   {
      Data msg("Expected a digit, got: ");
      msg += Data(mPosition, (mEnd - mPosition));
      fail(__FILE__, __LINE__,msg);
   }
   
   unsigned int num = 0;
   while (!eof() && isdigit(*mPosition))
   {
      num = num*10 + (*mPosition-'0');
      skipChar();
   }
   
   return num;
}

unsigned long long
ParseBuffer::unsignedLongLong()
{
   if (this->eof())
   {

      fail(__FILE__, __LINE__,"Expected a digit, got eof ");
   }

   const char* p = mPosition;
   assert(p);
   char c = *p;

   if (!isdigit(c))
   {
      Data msg("Expected a digit, got: ");
      msg += Data(mPosition, (mEnd - mPosition));
      fail(__FILE__, __LINE__,msg);
   }
   
   unsigned int num = 0;
   while (!eof() && isdigit(*mPosition))
   {
      num = num*10 + (*mPosition-'0');
      skipChar();
   }   
   return num;
}


float
ParseBuffer::floatVal()
{
   const char* s = mPosition;
   try
   {
      float mant = 0.0;
      int num = integer();

      if (*mPosition == '.')
      {
	  skipChar();
	  const char* pos = mPosition;
	  mant = float(integer());
	  int s = mPosition - pos;
	  while (s--)
	  {
	     mant /= 10.0;
	  }
      }
      return num + mant;
   }
   catch (Exception&)
   {
     Data msg("Expected a floating point value, got: ");
     msg += Data(s, mPosition - s);
     fail(__FILE__, __LINE__,msg);
     return 0.0;
   }
}

void
ParseBuffer::assertEof() const
{
   if (!eof())
   {

      fail(__FILE__, __LINE__,"expected eof");
   }      
}

void
ParseBuffer::assertNotEof() const
{
   if (eof())
   {
      fail(__FILE__, __LINE__,"unexpected eof");
   }      
}

Data
spaces(unsigned int numSpaces)
{
   Data sps(numSpaces, true);
   for (unsigned int i = 0; i < numSpaces; i++)
   {
      sps += ' ';
   }
   return sps;
}

Data 
escapeAndAnnotate(const char* buffer, 
                  unsigned int size,
                  const char* position)
{ 
   Data ret(2*size+16, true);

   const char* lastReturn = buffer;
   int lineCount = 0;
   bool doneAt = false;

   const char* p = buffer;
   for (unsigned int i = 0; i < size; i++)
   {
      unsigned char c = *p++;

      switch (c)
      {
         case 0x0D: // CR
         {
            continue;
         }
         case 0x0A: // LF
         {
            if (!doneAt && p >= position)
            {
               ret += "[CRLF]\n";
               ret += spaces((unsigned int)(position - lastReturn));
               ret += "^[CRLF]\n";
               doneAt = true;
            }
            else
            {
               lastReturn = p;
               ret += c;
            }
            lineCount++;
            continue;
         }
      }
      
      if (iscntrl(c) || (c >= 0x7F))
      {
         ret +='*'; // indicates unprintable character
         continue;
      }

      ret += c;
   }
   if (!doneAt && p >= position)
   {
      ret += "\n";
      ret += spaces((unsigned int)(position - lastReturn));
      ret += "^\n";
   }

   return ret;
}

void
ParseBuffer::fail(const char* file, unsigned int line, const Data& detail) const
{
    Data errmsg;
    DataStream ds(errmsg);
    ds << file << ":" << line
       << ", Parse failed ";

    if (detail != Data::Empty) ds << detail << ' ' ;

    ds << "in context: " << mErrorContext
       << std::endl
       << escapeAndAnnotate(mBuff, mEnd - mBuff, mPosition);
    ds.flush();

   throw Exception(errmsg, mErrorContext, file, line);
}

ParseBuffer::Exception::Exception(const Data& msg, const Data& context, const Data& file, const int line)
   : msrp::BaseException(msg, file, line) 
{}

ParseBuffer::Exception::~Exception() throw() 
{}

const char* 
ParseBuffer::Exception::name() const 
{ 
   return "ParseBuffer::Exception"; 
}

const Data& 
ParseBuffer::Exception::getContext() const
{
   return mContext;
}

ParseBuffer::Pointer::Pointer(const ParseBuffer& pb,
                              const char* position,
                              bool atEof)
   : mPb(pb),
     mPosition(position),
     mIsValid(!atEof)
{}

const char& 
ParseBuffer::Pointer::operator*() const
{
   if (mIsValid)
   {
      return *mPosition;
   }
   else
   {
      throw ParseBuffer::Exception(msg, mPb.getContext(), __FILE__, __LINE__);
   }
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
