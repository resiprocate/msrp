#if defined(HAVE_CONFIG_H)
#include "common/config.hxx"
#endif

#include "msrp_request_line.h"
//#include "src/UnknownParameter.hxx"
#include "common/os/Data.hxx"
//#include "src/os/DnsUtil.hxx"
#include "common/os/Logger.hxx"
#include "common/os/ParseBuffer.hxx"
#include "common/os/WinLeakCheck.hxx"

using namespace msrp;
using namespace std;

#define RESIPROCATE_SUBSYSTEM Subsystem::SIP
#define defineMsrpMethod(_enum, _name) _name

namespace msrp
{

Data MsrpMethodNames[] =
{
   defineMsrpMethod(SEND, "SEND"),
   defineMsrpMethod(REPORT, "REPORT"),
   defineMsrpMethod(OTHER, "OTHER")
};

}


//====================
// MsrpRequestLine:
//====================
MsrpRequestLine::MsrpRequestLine(MsrpMethodTypes method, 
                                 const Data& transactionId)
   : ParserCategory(),
     mMethod(method),
     mTransactionId(transactionId)
{
}

MsrpRequestLine::MsrpRequestLine(const Data& otherMethodName,
                                 const Data& transactionId)
   : ParserCategory(),
     mMethod(OTHER),
     mOtherMethodName(otherMethodName),
     mTransactionId(transactionId)
{   
}

MsrpRequestLine::MsrpRequestLine(HeaderFieldValue* hfv,
                                 Headers::Type type)
   : ParserCategory(hfv, type),
     mMethod(OTHER)
{
}

MsrpRequestLine::MsrpRequestLine(const MsrpRequestLine& rhs)
   : ParserCategory(rhs),
     mMethod(rhs.mMethod),
     mOtherMethodName(rhs.mOtherMethodName),
     mTransactionId(rhs.mTransactionId)
{
}

MsrpRequestLine&
MsrpRequestLine::operator=(const MsrpRequestLine& rhs)
{
   if (this != &rhs)
   {
      ParserCategory::operator=(rhs);
      mMethod = rhs.mMethod;
      mOtherMethodName = rhs.mOtherMethodName;
      mTransactionId = rhs.mTransactionId;
   }
   return *this;
}

MsrpRequestLine::~MsrpRequestLine()
{
}

ParserCategory *
MsrpRequestLine::clone() const
{
   return new MsrpRequestLine(*this);
}

MsrpMethodTypes& 
MsrpRequestLine::method() 
{
   checkParsed(); 
   return mMethod;
}

const MsrpMethodTypes&
MsrpRequestLine::method() const 
{
   checkParsed(); 
   return mMethod;
}

Data& 
MsrpRequestLine::otherMethodName()
{
   checkParsed(); 
   return mOtherMethodName;
}

const Data& 
MsrpRequestLine::otherMethodName() const 
{
   checkParsed(); 
   return mOtherMethodName;
}

Data&
MsrpRequestLine::transactionId()
{
   checkParsed();
   return mTransactionId;
}

const Data&
MsrpRequestLine::transactionId() const
{
   checkParsed();
   return mTransactionId;
}

void 
MsrpRequestLine::parse(ParseBuffer& pb)
{
   const char* start;
   pb.skipWhitespace();
   pb.skipNonWhitespace();
   start = pb.skipWhitespace();
   pb.skipNonWhitespace();
   pb.data(mTransactionId, start);
   start = pb.skipWhitespace();
   pb.skipNonWhitespace();
   mMethod = getMsrpMethodType(start, pb.position() - start);
   if (mMethod == OTHER)
   {
      pb.data(mOtherMethodName, start);
   }
   else
   {
      mOtherMethodName = "";
   }
}

ostream&
MsrpRequestLine::encodeParsed(ostream& str) const
{
   str << mTransactionId << Symbols::SPACE
       << (mMethod != OTHER ? getMsrpMethodName(mMethod) : mOtherMethodName);
   return str;
}

const Data&
MsrpRequestLine::getMsrpMethodName(MsrpMethodTypes t) const
{
   if (t < SEND || t >= MAX_MSRP_METHODS)
   {
      t = OTHER;
   }
   
   return MsrpMethodNames[t];
}

MsrpMethodTypes
MsrpRequestLine::getMsrpMethodType(const char* name,
                                   int len)
{
   if (0 == len) return OTHER;
   if ('S' == name[0]) return SEND;
   if ('R' == name[0]) return REPORT;
   return OTHER;
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
