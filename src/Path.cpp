#if defined(HAVE_CONFIG_H)
#include "common/config.hxx"
#endif

#include "Path.h"
#include "common/ParseException.hxx"
#include "common/UnknownParameter.hxx"
#include "common/os/Data.hxx"
#include "common/os/DnsUtil.hxx"
#include "common/os/Logger.hxx"
#include "common/os/ParseBuffer.hxx"
#include "common/os/WinLeakCheck.hxx"

using namespace msrp;
using namespace std;

#define RESIPROCATE_SUBSYSTEM Subsystem::SIP

//====================
// Path:
//====================
Path::Path() : 
   ParserCategory()
{}

Path::Path(HeaderFieldValue* hfv,
           Headers::Type type)
   : ParserCategory(hfv, type)
{}

Path::Path(const Path& rhs)
   : ParserCategory(rhs),
     mUrl(rhs.mUrl),
     mUrls(rhs.mUrls)
{}


Path::Path(const MsrpUrl& msrpUrl)
   : ParserCategory(),
     mUrl(msrpUrl)
{}

Path::~Path()
{}

Path&
Path::operator=(const Path& rhs)
{
   if (this != &rhs)
   {
      assert( &rhs != 0 );
      
      ParserCategory::operator=(rhs);
      mUrl = rhs.mUrl;
      mUrls = rhs.mUrls;
   }
   return *this;
}

bool
Path::operator==(const Path& other) const
{
   return mUrl==other.mUrl && mUrls==other.mUrls;
}

bool
Path::operator!=(const Path& other) const
{
   return !(*this==other);
}

ParserCategory *
Path::clone() const
{
   return new Path(*this);
}

const MsrpUrl&
Path::msrpUrl() const 
{
   checkParsed(); 
   return mUrl;
}

MsrpUrl&
Path::msrpUrl()
{
   checkParsed(); 
   return mUrl;
}

const Data&
Path::urls() const
{
   checkParsed();
   return mUrls;
}

Data&
Path::urls()
{
   checkParsed();
   return mUrls;
}

void
Path::parse(ParseBuffer& pb)
{
   const char* start;
   start = pb.skipWhitespace();
   pb.assertNotEof();   
   pb.skipNonWhitespace();
   ParseBuffer pbuf(start, pb.position() - start);
   mUrl.parse(pbuf);
   pb.skipWhitespace();
   if (!pb.eof())
   {
      start = pb.position();
      pb.skipToEnd();
      pb.data(mUrls, start);
   }
   else
   {
      mUrls = "";
   }
}

ostream&
Path::encodeParsed(ostream& str) const
{
   mUrl.encodeParsed(str);
   if (!mUrls.empty())
   {
      str << Symbols::SPACE << mUrl;
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
