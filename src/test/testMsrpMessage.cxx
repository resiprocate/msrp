
#include <iostream>
#include <memory>
#include <list>
#include <assert.h>

#include "src/MsrpMessage.h"
#include "common/os/DataStream.hxx"
#include "common/os/Logger.hxx"
#include "common/os/DnsUtil.hxx"
#include "common/os/ParseBuffer.hxx"

using namespace msrp;
using namespace std;

#define CRLF "\r\n"

#define RESIPROCATE_SUBSYSTEM Subsystem::TEST

int
main(int argc, char* argv[])
{
  //tassert_init(4);

  Log::initialize(Log::Cout, Log::Info, argv[0]);

  {  
     // Request.
     //
     Data txt("MSRP d93kswow SEND\r\n"
        "To-Path:msrp:bob.example.com:8888/9di4ea;tcp\r\n"
        "From-Path:msrp:alicepc.example.com:7777/iau39;tcp\r\n"
        "Message-ID: 1239sdqwer\r\n"
        "Content-Type:text/plain\r\n"
        "\r\n"
        "Hi, I'm Alice.\r\n"
        "-------d93kswow$\r\n");
     try
     {
        auto_ptr<MsrpMessage> msg(MsrpMessage::make(txt.c_str(), false));
        Data id = msg->getTransactionId();
        assert("d93kswow"==id);
        RequestLineType r;
        assert(msg->header(r).transactionId()=="d93kswow");
        assert(msg->header(r).method()==SEND);
        assert(msg->header(r).otherMethodName()=="");
        assert(msg->header(h_ToPath).msrpUrl().port()==8888);
        assert(msg->header(h_ToPath).msrpUrl().transport()=="tcp");
        assert(msg->header(h_ToPath).msrpUrl().host()=="bob.example.com");
        assert(msg->header(h_ToPath).msrpUrl().user()=="");
        assert(msg->header(h_ToPath).msrpUrl().scheme()=="msrp");
        assert(msg->header(h_ToPath).msrpUrl().sessionId()=="9di4ea");
        cout << msg->header(h_ToPath) << endl;

        assert(msg->header(h_FromPath).msrpUrl().port()==7777);
        assert(msg->header(h_FromPath).msrpUrl().transport()=="tcp");
        assert(msg->header(h_FromPath).msrpUrl().host()=="alicepc.example.com");
        assert(msg->header(h_FromPath).msrpUrl().scheme()=="msrp");
        assert(msg->header(h_FromPath).msrpUrl().sessionId()=="iau39");
        cout << msg->header(h_FromPath) << endl;

        assert(msg->header(h_MessageId).value()=="1239sdqwer");
        cout << msg->header(h_MessageId) << endl;

        assert(msg->header(h_ContentType).type()=="text");
        assert(msg->header(h_ContentType).subType()=="plain");
        cout << msg->header(h_ContentType) << endl;

        Contents* pContents = msg->getContents();
        cout << pContents->getBodyData() << endl;
     }
     catch (BaseException& e)
     {
        assert(false);
     }
  
  }

  {
     // Response.
     //
     Data txt("MSRP d93kswow 200 OK\r\n"
        "To-Path:msrp:bob.example.com:8888/9di4ea;tcp\r\n"
        "From-Path:msrp:alicepc.example.com:7777/iau39;tcp\r\n"
        "\r\n"
        "-------d93kswow$\r\n");
     try
     {
        auto_ptr<MsrpMessage> msg(MsrpMessage::make(txt.c_str(), false));
        StatusLineType s;
        assert(msg->getTransactionId()=="d93kswow");
        assert(msg->header(s).phrase()=="OK");
        assert(msg->header(s).statusCode()==200);

        assert(msg->header(h_ToPath).msrpUrl().port()==8888);
        assert(msg->header(h_ToPath).msrpUrl().transport()=="tcp");
        assert(msg->header(h_ToPath).msrpUrl().host()=="bob.example.com");
        assert(msg->header(h_ToPath).msrpUrl().user()=="");
        assert(msg->header(h_ToPath).msrpUrl().scheme()=="msrp");
        assert(msg->header(h_ToPath).msrpUrl().sessionId()=="9di4ea");
        cout << msg->header(h_ToPath) << endl;

        assert(msg->header(h_FromPath).msrpUrl().port()==7777);
        assert(msg->header(h_FromPath).msrpUrl().transport()=="tcp");
        assert(msg->header(h_FromPath).msrpUrl().host()=="alicepc.example.com");
        assert(msg->header(h_FromPath).msrpUrl().scheme()=="msrp");
        assert(msg->header(h_FromPath).msrpUrl().sessionId()=="iau39");
        cout << msg->header(h_FromPath) << endl;
     }
     catch (BaseException& e)
     {
        assert(false);
     }
  }

   return 0;
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
