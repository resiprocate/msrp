#if !defined(MSRP_HEADERTYPES_HXX)
#define MSRP_HEADERTYPES_HXX 

#include "common/os/Data.hxx"

//****************************************************************************
//
// !dlb! until automated, must ensure that this set is consistent with
//
// HeaderTypes.hxx
// Headers.hxx
// HeaderHash.gperf
// Headers.cxx
// MsrpMessage.hxx
// MsrpMessage.cxx
//
//****************************************************************************

// eventually use these macros to automate Headers.hxx, Headers.cxx+gperf
#define UNUSED_defineHeader(_enum, _name, _type, _rfc) SAVE##_enum, _enum = UNKNOWN, RESET##enum = SAVE##_enum-1
#define UNUSED_defineMultiHeader(_enum, _name, _type, _rfc) SAVE##_enum, _enum = UNKNOWN, RESET##enum = SAVE##_enum-1
#define defineHeader(_enum, _name, _type, _rfc) _enum
#define defineMultiHeader(_enum, _name, _type, _rfc) _enum

namespace msrp
{

class Headers
{
   public:
      // put headers that you want to appear early in the message early in
      // this set
      enum Type
      {
         UNKNOWN = -1,
         
         defineHeader(ToPath, "To-Path", Path, "RFC ????"),
         defineHeader(FromPath, "From-Path", Path, "RFC ????"),
         defineHeader(MessageId, "Message-ID", Token, "RFC ????"),
         defineHeader(ReportSuccess, "Report-Success", ReprotSuccess, "RFC ????"),
         defineHeader(ReportFailure, "Report-Failure", ReportFailure, "RFC ????"),
         defineHeader(ByteRange, "Byte-Range", ByteRange, "RFC ????"),
         defineHeader(Status, "Status", Status, "RFC ????"),
         defineHeader(ContentEncoding, "Content-Encoding", Token, "RFC ????"),
         defineHeader(ContentId, "Content-ID", Token, "RFC 2045"),
         defineHeader(ContentTransferEncoding, "Content-Transfer-Encoding", StringCategory, "RFC ????"), // !dlb! defineMultiHeader
         defineHeader(ContentType, "Content-Type", Mime, "RFC ????"),
         defineHeader(ContentLength, "Content-Length", IntegerCategory, "RFC ????"),
         defineHeader(ContentDisposition, "Content-Disposition", Token, "RFC 2045"),
         defineHeader(MIMEVersion, "MIME-Version", Token, "RFC 2045"),         
         defineMultiHeader(ContentLanguage, "Content-Language", Token, "RFC ????"),
         MAX_HEADERS,
         NONE
      };

      // get enum from header name
      static Type getType(const char* name, int len);
      static bool isCommaTokenizing(Type type);
      static bool isCommaEncoding(Type type);
      static const Data& getHeaderName(int);

      // treat as private
      static bool CommaTokenizing[MAX_HEADERS+1];
      static bool CommaEncoding[MAX_HEADERS+1];
      static Data HeaderNames[MAX_HEADERS+1];
};
 
}

#undef UNUSED_defineHeader
#undef UNUSED_defineMultiHeader
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
