#if defined(HAVE_CONFIG_H)
#include "common/config.hxx"
#endif

#include "MsrpStatusLine.h"
#include "common/os/Data.hxx"
#include "common/os/ParseBuffer.hxx"
#include "common/os/WinLeakCheck.hxx"

using namespace msrp;
using namespace std;

#define RESIPROCATE_SUBSYSTEM Subsystem::SIP

//====================
// MsrpStatusLine:
//====================
MsrpStatusLine::MsrpStatusLine(const Data& transactionId,
                               const int statusCode)
   : ParserCategory(), 
     mTransactionId(transactionId), 
     mStatusCode(statusCode)
{
}

MsrpStatusLine::MsrpStatusLine(const Data& transactionId,
                               const int statusCode,
                               const Data& phrase)
   : ParserCategory(),
     mTransactionId(transactionId),
     mStatusCode(statusCode),
     mPhrase(phrase)
{
}

MsrpStatusLine::MsrpStatusLine(HeaderFieldValue* hfv, 
                               Headers::Type type)
   : ParserCategory(hfv, type),
     mStatusCode(-1)
{
}

MsrpStatusLine::MsrpStatusLine(const MsrpStatusLine& rhs)
   : ParserCategory(rhs),
     mTransactionId(rhs.mTransactionId),
     mStatusCode(rhs.mStatusCode),
     mPhrase(rhs.mPhrase)
{
}

MsrpStatusLine&
MsrpStatusLine::operator=(const MsrpStatusLine& rhs)
{
   if (this != &rhs)
   {
      ParserCategory::operator=(rhs);
      mTransactionId = rhs.mTransactionId;
      mStatusCode = rhs.mStatusCode;
      mPhrase = rhs.mPhrase;
   }
   return *this;
}

MsrpStatusLine::~MsrpStatusLine()
{
}

ParserCategory *
MsrpStatusLine::clone() const
{
   return new MsrpStatusLine(*this);
}

Data&
MsrpStatusLine::transactionId()
{
   checkParsed();
   return mTransactionId;
}

const Data&
MsrpStatusLine::transactionId() const
{
   checkParsed();
   return mTransactionId;
}

int&
MsrpStatusLine::statusCode()
{
   checkParsed();
   return mStatusCode;
}

const int&
MsrpStatusLine::statusCode() const
{
   checkParsed();
   return mStatusCode;
}

Data&
MsrpStatusLine::phrase()
{
   checkParsed();
   return mPhrase;
}

const Data&
MsrpStatusLine::phrase() const
{
   checkParsed();
   return mPhrase;
}

void 
MsrpStatusLine::parse(ParseBuffer& pb)
{
   const char* start;
   pb.skipWhitespace();
   pb.skipNonWhitespace();
   start = pb.skipWhitespace();
   pb.skipNonWhitespace();
   pb.data(mTransactionId, start);
   start = pb.skipWhitespace();
   mStatusCode = pb.integer();
   pb.skipNonWhitespace();
   pb.skipWhitespace();
   if (!pb.eof())
   {
      start = pb.position();
      pb.skipNonWhitespace();
      pb.data(mPhrase, start);
   }
   else
   {
      mPhrase = "";
   }
}

ostream&
MsrpStatusLine::encodeParsed(ostream& str) const
{
   str << mTransactionId << Symbols::SPACE << mStatusCode;
   if ( !mPhrase.empty() ) str << Symbols::SPACE << mPhrase;
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
