#if defined(HAVE_CONFIG_H)
#include "src/config.hxx"
#endif

#include "src/ReportSuccess.hxx"
#include "src/os/Data.hxx"
//#include "src/os/DnsUtil.hxx"
#include "src/os/Logger.hxx"
#include "src/os/ParseBuffer.hxx"
#include "src/os/WinLeakCheck.hxx"

using namespace msrp;
using namespace std;

#define RESIPROCATE_SUBSYSTEM Subsystem::SIP


//====================
// ReportSuccess
//===================
ReportSuccess::ReportSuccess() 
   : ParserCategory(), 
     mValue(false) 
{}
  
ReportSuccess::ReportSuccess(HeaderFieldValue* hfv, Headers::Type type) 
   : ParserCategory(hfv, type),
     mValue(false) 
{}

ReportSuccess::ReportSuccess(const ReportSuccess& rhs)
   : ParserCategory(rhs),
     mValue(rhs.mValue)
{}

ReportSuccess&
ReportSuccess::operator=(const ReportSuccess& rhs)
{
   if (this != &rhs)
   {
      ParserCategory::operator=(rhs);
      mValue = rhs.mValue;
   }
   return *this;
}

bool
ReportSuccess::operator==(const ReportSuccess& rhs) const
{
   return (success() == rhs.success());
}

bool& 
ReportSuccess::success() const 
{
   checkParsed(); 
   return mValue;
}

void
ReportSuccess::parse(ParseBuffer& pb)
{
   pb.skipWhitespace();
   switch (*pb.position())
   {
      case 'y':
         mValue = true;
         break;
      case 'n':
         mValue = false;
         break;
      default:
         pb.fail(__FILE__, __LINE__, "invalid report success");
   }
}

ParserCategory* 
ReportSuccess::clone() const
{
   return new ReportSuccess(*this);
}

std::ostream& 
ReportSuccess::encodeParsed(std::ostream& str) const
{
   str << mValue? "yes" : "no";
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
