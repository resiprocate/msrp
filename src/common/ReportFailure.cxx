#if defined(HAVE_CONFIG_H)
#include "msrp/config.hxx"
#endif

#include "common/ReportFailure.hxx"
#include "common/os/Data.hxx"
//#include "common/os/DnsUtil.hxx"
#include "common/os/Logger.hxx"
#include "common/os/ParseBuffer.hxx"
#include "common/os/WinLeakCheck.hxx"

using namespace msrp;
using namespace std;

#define RESIPROCATE_SUBSYSTEM Subsystem::SIP


//====================
// ReportFailure
//===================
ReportFailure::ReportFailure() 
   : LazyParser(),//ParserCategory(), 
     mValue(Yes) 
{}
  
ReportFailure::ReportFailure(HeaderFieldValue* hfv, Headers::Type type) 
   : LazyParser(hfv),//ParserCategory(hfv, type),
     mValue(Yes) 
{}

ReportFailure::ReportFailure(const ReportFailure& rhs)
   : LazyParser(rhs), //ParserCategory(rhs),
     mValue(rhs.mValue)
{}

ReportFailure&
ReportFailure::operator=(const ReportFailure& rhs)
{
   if (this != &rhs)
   {
      //ParserCategory::operator=(rhs);
      LazyParser::operator=(rhs);
      mValue = rhs.mValue;
   }
   return *this;
}

bool
ReportFailure::operator==(const ReportFailure& rhs) const
{
   return (result() == rhs.result());
}

ReportFailure::Result& 
ReportFailure::result() const
{
   checkParsed(); 
   return mValue;
}

void
ReportFailure::parse(ParseBuffer& pb)
{
   pb.skipWhitespace();
   switch (*pb.position())
   {
      case 'y':
         mValue = Yes;
         break;
      case 'n':
         mValue = No;
         break;
      case 'p':
         mValue = Partial;
         break;
      default:
         pb.fail(__FILE__, __LINE__, "invalid report failure");
   }
}

/*
ParserCategory* 
ReportFailure::clone() const
{
   return new ReportFailure(*this);
}
*/

std::ostream& 
ReportFailure::encodeParsed(std::ostream& str) const
{
   switch ( mValue )
   {
      case Yes:
         str << "yes";
         break;
      case No:
         str << "no";
         break;
      case Partial:
         str << "partial";
         break;
      default:
         break;
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
