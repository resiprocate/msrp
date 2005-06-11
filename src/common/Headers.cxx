#if defined(HAVE_CONFIG_H)
#include "common/config.hxx"
#endif

#include "common/os/Data.hxx"
#include "common/Headers.hxx"
#include "common/Symbols.hxx"
#include "MsrpRoar.h"

// GPERF generated external routines
#include "common/HeaderHash.hxx"

#include <iostream>
using namespace std;

//int strcasecmp(const char*, const char*);
//int strncasecmp(const char*, const char*, int len);

using namespace msrp;

Data Headers::HeaderNames[MAX_HEADERS+1];
bool Headers::CommaTokenizing[] = {false};
bool Headers::CommaEncoding[] = {false};

bool 
Headers::isCommaTokenizing(Type type)
{
   return CommaTokenizing[type+1];
}

bool 
Headers::isCommaEncoding(Type type)
{
   return CommaEncoding[type+1];
}

const Data&
Headers::getHeaderName(int type)
{
   return HeaderNames[type+1];
}

#define defineHeader(_enum, _name, _type)                                                                               \
Headers::Type                                                                                                           \
H_##_enum::getTypeNum() const {return Headers::_enum;}                                                                  \
                                                                                                                        \
H_##_enum::H_##_enum()                                                                                                  \
{                                                                                                                       \
   Headers::CommaTokenizing[Headers::_enum+1] = bool(Type::commaHandling & ParserCategory::CommasAllowedOutputMulti);   \
   Headers::CommaEncoding[Headers::_enum+1] = bool(Type::commaHandling & 2);                                            \
   Headers::HeaderNames[Headers::_enum+1] = _name;                                                                      \
}                                                                                                                       \
                                                                                                                        \
_type&                                                                                                                  \
H_##_enum::knownReturn(ParserContainerBase* container)                                                                  \
{                                                                                                                       \
   return dynamic_cast<ParserContainer<_type>*>(container)->front();                                                    \
}                                                                                                                       \
                                                                                                                        \
H_##_enum msrp::h_##_enum

#define defineMultiHeader(_enum, _name, _type)                                                                                         \
   Headers::Type                                                                                                                        \
H_##_enum##s::getTypeNum() const {return Headers::_enum;}                                                                               \
                                                                                                                                        \
H_##_enum##s::H_##_enum##s()                                                                                                            \
{                                                                                                                                       \
   Headers::CommaTokenizing[Headers::_enum+1] = bool(Type::value_type::commaHandling & ParserCategory::CommasAllowedOutputMulti);       \
   Headers::CommaEncoding[Headers::_enum+1] = bool(Type::value_type::commaHandling & 2);                                                \
   Headers::HeaderNames[Headers::_enum+1] = _name;                                                                                      \
}                                                                                                                                       \
                                                                                                                                        \
ParserContainer<_type>&                                                                                                                 \
H_##_enum##s::knownReturn(ParserContainerBase* container)                                                                               \
{                                                                                                                                       \
   return *dynamic_cast<ParserContainer<_type>*>(container);                                                                            \
}                                                                                                                                       \
                                                                                                                                        \
H_##_enum##s msrp::h_##_enum##s

defineHeader(MessageId, "Message-ID", Token);
defineHeader(ContentEncoding, "Content-Encoding", Token);
defineHeader(ContentTransferEncoding, "Content-Transfer-Encoding", StringCategory);
defineHeader(MIMEVersion, "Mime-Version", Token);
defineHeader(ContentDisposition, "Content-Disposition", Token);

defineHeader(ContentId, "Content-ID", Token);

defineMultiHeader(ContentLanguage, "Content-Language", Token);

//====================
// Path
//====================

defineHeader(ToPath, "To-Path", Path);
defineHeader(FromPath, "From-Path", Path);

//====================
// ReportSuccess
//====================

defineHeader(ReportSuccess, "Report-Success", ReportSuccess);

//====================
// ReportFailure
//====================

defineHeader(ReportFailure, "Report-Failure", ReportFailure);

//====================
// ByteRange
//====================

defineHeader(ByteRange, "Byte-Range", ByteRange);

//====================
// Status
//====================

defineHeader(Status, "Status", Status);

//====================
// Mime
//====================

defineHeader(ContentType, "Content-Type", Mime);

//====================
// IntegerCategory
//====================

// !dlb! not clear this needs to be exposed
defineHeader(ContentLength, "Content-Length", IntegerCategory);

RequestLineType msrp::h_RequestLine;
StatusLineType msrp::h_StatusLine;

Headers::Type
Headers::getType(const char* name, int len)
{
   struct headers* p;
   p = HeaderHash::in_word_set(name, len);
   return p ? Headers::Type(p->type) : Headers::UNKNOWN;
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
