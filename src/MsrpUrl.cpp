#if defined(HAVE_CONFIG_H)
#include "common/config.hxx"
#endif

#include <set>

#include "common/Symbols.hxx"
#include "MsrpUrl.h"
#include "common/os/DataStream.hxx"
#include "common/os/DnsUtil.hxx"
#include "common/os/Logger.hxx"
#include "common/os/ParseBuffer.hxx"
#include "common/os/WinLeakCheck.hxx"

using namespace msrp;

#define RESIPROCATE_SUBSYSTEM Subsystem::SIP

MsrpUrl::MsrpUrl() 
   : ParserCategory(),
     mPort(0)
{
}

MsrpUrl::MsrpUrl(const Data& data)
   : ParserCategory(), 
     mPort(0)
{
   try
   {
      ParseBuffer pb(data.data(), data.size());
      MsrpUrl tmp;

      // avoid the destructor/constructor issue
      tmp.parse(pb);
      *this = tmp;
   }
   catch(ParseBuffer::Exception& e)
   {
      DebugLog(<< "Failed trying to construct a MsrpUrl from " << data << ": " << e);
      throw;
   }
}

MsrpUrl::MsrpUrl(const MsrpUrl& rhs)
   : ParserCategory(rhs),
     mScheme(rhs.mScheme),
     mHost(rhs.mHost),
     mUser(rhs.mUser),
     mPort(rhs.mPort),
     mSessionId(rhs.mSessionId),
     mTransport(rhs.mTransport)
{}

MsrpUrl::~MsrpUrl()
{}

MsrpUrl&
MsrpUrl::operator=(const MsrpUrl& rhs)
{
   if (this != &rhs)
   {
      ParserCategory::operator=(rhs);
      mScheme = rhs.mScheme;
      mHost = rhs.mHost;
      mUser = rhs.mUser;
      mPort = rhs.mPort;
      mSessionId = rhs.mSessionId;
      mTransport =rhs.mTransport;
   }
   return *this;
}

bool 
MsrpUrl::operator==(const MsrpUrl& other) const
{
   checkParsed();
   other.checkParsed();

   // compare hosts
   if (DnsUtil::isIpV6Address(mHost) &&
       DnsUtil::isIpV6Address(other.mHost))
   {
      //no caching
      // compare canonicalized IPV6 addresses
      if (DnsUtil::canonicalizeIpV6Address(mHost) != DnsUtil::canonicalizeIpV6Address(other.mHost))
      {
         return false;
      }
   }
   else
   {
      if ( DnsUtil::isIpV4Address(mHost) &&
           DnsUtil::isIpV4Address(other.mHost))
      {
         if (mHost != other.mHost)
         {
            return false;
         }
         
      }
      else 
      {
         if (!isEqualNoCase(mHost, other.mHost))
         {
            return false;
         }
      }
   }
   
   return mScheme == other.mScheme && 
      mPort == other.mPort &&
      mSessionId == other.mSessionId &&
      mTransport == other.mTransport;
}
   
bool 
MsrpUrl::operator!=(const MsrpUrl& other) const
{
   return !(*this == other);
}

bool
MsrpUrl::operator<(const MsrpUrl& other) const
{
   other.checkParsed();
   checkParsed();
   if (mUser < other.mUser)
   {
      return true;
   }
   if (mUser > other.mUser)
   {
      return false;
   }

   //!dcm! -- v6?
   if (mHost < other.mHost)
   {
      return true;
   }
   if (mHost > other.mHost)
   {
      return false;
   }

   if (mPort < other.mPort)
   {
      return true;
   }
   if (mPort > other.mPort)
   {
      return false;
   }

   if (mSessionId < other.mSessionId)
   {
      return true;
   }
   if (mSessionId > other.mSessionId)
   {
      return false;
   }
   return mTransport< other.mTransport;
}

void
MsrpUrl::parse(ParseBuffer& pb)
{
   pb.skipWhitespace();
   const char* start = pb.position();
   pb.skipToOneOf(":@"); // make sure the colon precedes

   pb.assertNotEof();

   pb.data(mScheme, start);
   pb.skipChar(Symbols::COLON[0]);
   mScheme.lowercase(); // case insensitive?

   start = pb.position();
   pb.skipToChar(Symbols::AT_SIGN[0]);
   if (!pb.eof())
   {
      pb.data(mUser, start);
      start = pb.skipChar(Symbols::AT_SIGN[0]);
   }
   else
   {
      pb.reset(start);
   }
      
   // !dcm! xten branch vs main branch
   if (*start == '[')
   {
      start = pb.skipChar();
      pb.skipToChar(']');
      pb.data(mHost, start);
      DnsUtil::canonicalizeIpV6Address(mHost);
      pb.skipChar();
      pb.skipToOneOf(":/;");
   }
   else
   {
      pb.skipToOneOf(":/;");      
      pb.data(mHost, start);
   }

   if (*pb.position() == Symbols::COLON[0])
   {
      pb.skipChar();
	  mPort = pb.integer();
      pb.skipToOneOf("/;");      
   }
         
   if (*pb.position() == Symbols::SLASH[0])
   {
      pb.skipChar();      
      start = pb.position();
      pb.skipToChar(Symbols::SEMI_COLON[0]);
      pb.data(mSessionId, start);
   }
   
   pb.skipChar(Symbols::SEMI_COLON[0]);
   if (pb.eof())
   {
      pb.fail(__FILE__, __LINE__, "no transport parameter found after semi-colon");
   }

   start = pb.position();   
   pb.skipToEnd();
   pb.data(mTransport, start);
}

ParserCategory*
MsrpUrl::clone() const
{
   return new MsrpUrl(*this);
}
 
// should not encode user parameters unless its a tel?
std::ostream& 
MsrpUrl::encodeParsed(std::ostream& str) const
{
   str << mScheme << Symbols::COLON; 
   if (!mUser.empty())
   {
      str << mUser;
   }
   if (!mHost.empty())
   {
     if (!mUser.empty())
     {
       str << Symbols::AT_SIGN;
     }
     if (DnsUtil::isIpV6Address(mHost))
     {
        str << '[' << mHost << ']';
     }
     else
     {
        str << mHost;
     }
   }
   if (mPort != 0)
   {
      str << Symbols::COLON << mPort;
   }
   if (!mSessionId.empty())
   {
      str << Symbols::SLASH << mSessionId;
   }
   str << Symbols::SEMI_COLON << mTransport;

   return str;
}

#if defined(HASH_MAP_NAMESPACE)
size_t HASH_MAP_NAMESPACE::hash<resip::MsrpUrl>::operator()(const resip::MsrpUrl& v) const
{
   return resip::Data::from(v).hash();
}
#elif defined(__INTEL_COMPILER)
size_t std::hash_value(const resip::MsrpUrl& v)
{
   return resip::Data::from(v).hash();
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
