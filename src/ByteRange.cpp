#if defined(HAVE_CONFIG_H)
#include "common/config.h"
#endif

#include "src/ByteRange.h"
#include "common/os/Data.hxx"
#include "common/os/Logger.hxx"
#include "common/os/ParseBuffer.hxx"
#include "common/os/WinLeakCheck.hxx"

using namespace msrp;
using namespace std;

#define RESIPROCATE_SUBSYSTEM Subsystem::SIP


//====================
// ByteRange
//===================
ByteRange::ByteRange() 
   : ParserCategory(), 
     mStart(0),
     mEnd(0),
     mTotal(0),
     mEndInt(true),
     mTotalInt(true)
{}
  
ByteRange::ByteRange(HeaderFieldValue* hfv, Headers::Type type) 
   : ParserCategory(hfv, type), 
     mStart(0),
     mEnd(0),
     mTotal(0),
     mEndInt(true),
     mTotalInt(true)
{}

ByteRange::ByteRange(const ByteRange& rhs)
   : ParserCategory(rhs),
     mStart(rhs.mStart),
     mEnd(rhs.mEnd),
     mTotal(rhs.mTotal),
     mEndInt(rhs.mEndInt),
     mTotalInt(rhs.mTotalInt)
{}

ByteRange&
ByteRange::operator=(const ByteRange& rhs)
{
   if (this != &rhs)
   {
      ParserCategory::operator=(rhs);
      mStart = rhs.mStart;
      mEnd = rhs.mEnd;
      mTotal = rhs.mTotal;
      mEndInt = rhs.mEndInt;
      mTotalInt = rhs.mTotalInt;
   }
   return *this;
}

bool
ByteRange::operator==(const ByteRange& rhs) const
{
   return (start() == rhs.start());
}

bool
ByteRange::operator<(const ByteRange& rhs) const
{
   return ( start() < rhs.start());
}

bool&
ByteRange::endInt() const
{
   checkParsed();
   return mEndInt;
}

bool&
ByteRange::totalInt() const
{
   checkParsed();
   return mTotalInt;
}

int&
ByteRange::end() const
{
   checkParsed();
   return mEnd;
}

int&
ByteRange::total() const
{
   checkParsed();
   return mTotal;
}

int&
ByteRange::start() const 
{
   checkParsed(); 
   return mStart;
}

void
ByteRange::parse(ParseBuffer& pb)
{
   const char* start = pb.skipWhitespace();
   mStart = pb.integer();
   pb.skipToChar(Symbols::DASH[0]);
   start = pb.skipChar(Symbols::DASH[0]);
   if (*pb.position() == Symbols::STAR[0])
   {
      mEnd = 0;
      mEndInt = false;
   }
   else
   {
      mEnd = pb.integer();
      mEndInt = true;
   }   
   pb.skipToChar(Symbols::SLASH[0]);
   start = pb.skipChar(Symbols::SLASH[0]);
   if (*pb.position() == Symbols::STAR[0])
   {
      mTotal = 0;
      mTotalInt = false;
   }
   else
   {
      mTotal = pb.integer();
      mTotalInt = true;
   }
}

ParserCategory* 
ByteRange::clone() const
{
   return new ByteRange(*this);
}

std::ostream& 
ByteRange::encodeParsed(std::ostream& str) const
{
   str << mStart << Symbols::DASH;

   if ( mEndInt )
   {
      str <<  mEnd;
   }
   else
   {
      str << Symbols::STAR;
   }

   str << Symbols::SLASH;

   if ( mTotalInt )
   {
      str << mTotal;
   }
   else
   {
      str << Symbols::STAR;
   }

   return str;
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
