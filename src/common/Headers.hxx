#if !defined(MSRP_HEADERS_HXX)
#define MSRP_HEADERS_HXX 

#include "common/ParserCategories.hxx"
#include "common/HeaderTypes.hxx"
#include "common/Symbols.hxx"
#include "common/os/Data.hxx"
#include "common/os/HeapInstanceCounter.hxx"

namespace msrp
{
//class MsrpMessage;

//#define PARTIAL_TEMPLATE_SPECIALIZATION
#ifdef PARTIAL_TEMPLATE_SPECIALIZATION
template<bool>
class TypeIf
{
   public:
      template <class T>
      class Resolve
      {
         public:
            typedef T Type;
      };
};

class UnusedHeader
{
};

class TypeIf<false>
{
   public:
      template <class T>
      class Resolve
      {
         public:
            typedef UnusedHeader Type;
      };
};

#define UnusedChecking(_enum)                                           \
      typedef TypeIf<Headers::_enum != Headers::UNKNOWN> TypeIfT;       \
      typedef TypeIfT::Resolve<Type> Resolver;                          \
      typedef Resolver::Type UnknownReturn

#define MultiUnusedChecking(_enum)                                              \
      typedef TypeIf<Headers::_enum != Headers::UNKNOWN> TypeIfT;               \
      typedef TypeIfT::Resolve< ParserContainer<Type> > Resolver;               \
      typedef Resolver::Type UnknownReturn

#else

#define UnusedChecking(_enum) typedef int _dummy
#define MultiUnusedChecking(_enum) typedef int _dummy

#endif

class HeaderBase
{
   public:
      virtual Headers::Type getTypeNum() const = 0;
};

#define defineHeader(_enum, _name, _type, _rfc)                 \
class H_##_enum : public HeaderBase                             \
{                                                               \
   public:                                                      \
      RESIP_HeapCount(H_##_enum);                               \
      enum {Single = true};                                     \
      typedef _type Type;                                       \
      UnusedChecking(_enum);                                    \
      static Type& knownReturn(ParserContainerBase* container); \
      virtual Headers::Type getTypeNum() const;                 \
      H_##_enum();                                              \
};                                                              \
extern H_##_enum h_##_enum

#define defineMultiHeader(_enum, _name, _type, _rfc)            \
class H_##_enum##s : public HeaderBase                          \
{                                                               \
   public:                                                      \
      RESIP_HeapCount(H_##_enum##s);                            \
      enum {Single = false};                                    \
      typedef ParserContainer<_type> Type;                      \
      MultiUnusedChecking(_enum);                               \
      static Type& knownReturn(ParserContainerBase* container); \
      virtual Headers::Type getTypeNum() const;                 \
      H_##_enum##s();                                           \
};                                                              \
extern H_##_enum##s h_##_enum##s

//====================
// Token:
//====================
typedef ParserContainer<Token> Tokens;

defineHeader(ContentEncoding, "Content-Encoding", Token, "RFC ????");
defineHeader(ContentId, "Content-ID", Token, "RFC 2045");
defineHeader(MessageId, "Message-ID", Token, "RFC ????");
defineHeader(MIMEVersion, "MIME-Version", Token, "RFC 2045");
defineHeader(ContentDisposition, "Content-Disposition", Token, "RFC 2045");

defineMultiHeader(ContentLanguage, "Content-Language", Token, "RFC ????");

// Path
//====================
typedef ParserContainer<Path> Paths;

defineHeader(ToPath, "To-Path", Path, "RFC ????");
defineHeader(FromPath, "From-Path", Path, "RFC ????");

//====================
// ReportSuccess
//====================
typedef ParserContainer<ReportSuccess> ReportSuccesses;

defineHeader(ReportSuccess, "Report-Success", ReportSuccess, "RFC ????");

//====================
// ReportFailure
//====================
typedef ParserContainer<ReportFailure> ReportFailures;

defineHeader(ReportFailure, "Report-Failure", ReportFailure, "RFC ????");

//====================
// ByteRange
//====================
typedef ParserContainer<ByteRange> ByteRanges;

defineHeader(ByteRange, "Byte-Range", ByteRange, "RFC ????");

//====================
// Status
//====================
typedef ParserContainer<Status> Statuses;

defineHeader(Status, "Status", Status, "RFC ????");

//====================
// StringCategory:
//====================

defineHeader(ContentTransferEncoding, "Content-Transfer-Encoding", StringCategory, "RFC ????");

//====================
// Mime
//====================
typedef ParserContainer<Mime> Mimes;

defineHeader(ContentType, "Content-Type", Mime, "RFC ????");

//====================
// IntegerCategory:
//====================
typedef ParserContainer<IntegerCategory> IntegerCategories;

// !dlb! not clear this needs to be exposed
defineHeader(ContentLength, "Content-Length", IntegerCategory, "RFC ????");

class RequestLineType {};
extern RequestLineType h_RequestLine;

class StatusLineType {};
extern StatusLineType h_StatusLine;
 
}

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
