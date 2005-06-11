#include <memory>
#include <iostream>
#include <assert.h>

#include "MsrpUrl.h"
#include "common/os/DataStream.hxx"
#include "common/os/Logger.hxx"
#include "common/os/DnsUtil.hxx"
#include "common/os/ParseBuffer.hxx"

using namespace msrp;
using namespace std;

#define RESIPROCATE_SUBSYSTEM Subsystem::TEST

int
main(int argc, char* argv[])
{
   Log::Level l = Log::Debug;
   Log::initialize(Log::Cerr, l, argv[0]);

   {
      MsrpUrl msrpUrl("msrp:user@192.168.0.123:9000/kjfjan;tcp");

      assert(msrpUrl.host() == "192.168.0.123");
      assert(msrpUrl.user() == "user");
      assert(msrpUrl.port() == 9000);
      assert(msrpUrl.sessionId() == "kjfjan");
      assert(msrpUrl.transport() == "tcp");
   }
#if 0
   {
      MsrpUrl msrpUrl("msrp:user@[5f1b:df00:ce3e:e200:20:800:121.12.131.12]");

      cerr << "!! " << msrpUrl.host() << endl;
      assert(msrpUrl.host() == "5f1b:df00:ce3e:e200:20:800:121.12.131.12");
      cerr << "!! " << Data::from(msrpUrl) << endl;
      assert(Data::from(msrpUrl) == "msrp:user@[5f1b:df00:ce3e:e200:20:800:121.12.131.12]");
   }
   
   {
      MsrpUrl msrpUrl("msrps:192.168.2.12");

      assert(msrpUrl.scheme() == "sips");
      assert(msrpUrl.password() == "");
      assert(msrpUrl.userParameters() == "");
      assert(msrpUrl.host() == "192.168.2.12");
      assert(msrpUrl.port() == 0);
   }

   {
      MsrpUrl msrpUrl("msrps:host.foo.com");
      assert(msrpUrl.scheme() == "sips");
      assert(msrpUrl.password() == "");
      assert(msrpUrl.userParameters() == "");
      assert(msrpUrl.host() == "host.foo.com");
      assert(msrpUrl.port() == 0);
   }

   {
      MsrpUrl msrpUrl("msrp:user;x-v17:password@host.com:5555");

      cerr << "user!!" << msrpUrl.user() << endl;
      cerr << "password!!" << msrpUrl.password() << endl;
      cerr << "userParams!!" << msrpUrl.userParameters() << endl;

      assert(msrpUrl.scheme() == "sip");
      assert(msrpUrl.user() == "user;x-v17");
      assert(msrpUrl.password() == "password");
      assert(msrpUrl.userParameters() == "");
      assert(msrpUrl.host() == "host.com");
      assert(msrpUrl.port() == 5555);
   }

   {
      // test bad parses
      try
      {
         MsrpUrl("noscheme@foo.com:1202");
         assert(false);
      }
      catch (ParseBuffer::Exception& e)
      {
      }
   }

   {
      // test q comparison
      MsrpUrl w1("msrp:wombat@192.168.2.221:5062;transport=Udp;q=1.0");
      MsrpUrl w2("msrp:wombat@192.168.2.221:5063;transport=Udp;q=0.5");
      MsrpUrl w3("msrp:wombat@192.168.2.221:5063;transport=Udp;q=0.5");
      MsrpUrl w4("msrp:wombat@192.168.2.221:5063;transport=Udp");

      MsrpUrl::GreaterQ gtQ;

      assert(gtQ(w1, w2));
      assert(!gtQ(w2, w1));
      assert(!gtQ(w2, w3));
      assert(!gtQ(w3, w2));
      assert(!gtQ(w1, w4));
      assert(!gtQ(w4, w1));
      assert(gtQ(w4, w3));
   }
   
   {
      MsrpUrl w1("msrp:wombat@192.168.2.221:5062;transport=Udp");
      MsrpUrl w2("msrp:wombat@192.168.2.221:5063;transport=Udp");
      assert(w1 != w2);
      assert(w1 < w2);
   }
   {
      MsrpUrl tel("tel:+358-555-1234567;pOstd=pP2;isUb=1411");
      assert(tel.user() == "+358-555-1234567");

      assert(Data::from(tel) == "tel:+358-555-1234567;pOstd=pP2;isUb=1411");
   }

   {
      MsrpUrl tel("tel:+358-555-1234567;pOstd=pP2;isUb=1411");
      MsrpUrl sip(MsrpUrl::fromTel(tel, MsrpUrl("msrp:company.com")));

      cerr << "!! " << Data::from(sip) << endl;
      assert(Data::from(sip) == "msrp:+358-555-1234567;isub=1411;postd=pp2@company.com;user=phone");
   }

   {
      MsrpUrl tel("tel:+358-555-1234567;foo=bar;aaaa=baz;pOstd=pP2;isUb=1411");
      MsrpUrl sip(MsrpUrl::fromTel(tel, MsrpUrl("msrp:company.com")));

      cerr << "!! " << Data::from(sip) << endl;
      assert(Data::from(sip) == "msrp:+358-555-1234567;isub=1411;postd=pp2;aaaa=baz;foo=bar@company.com;user=phone");
   }
   
   {
      MsrpUrl tel("tel:+358-555-1234567;postd=pp22");
      MsrpUrl sip(MsrpUrl::fromTel(tel, MsrpUrl("msrp:foo.com")));
      assert(Data::from(sip) == "msrp:+358-555-1234567;postd=pp22@foo.com;user=phone");
   }
   {
      MsrpUrl tel1("tel:+358-555-1234567;postd=pp22");
      MsrpUrl tel2("tel:+358-555-1234567;POSTD=PP22");
      cerr << "tel1=" << tel1 << " user=" << tel1.user() << endl;
      cerr << "tel2=" << tel2 << " user=" << tel2.user() << endl;
      assert (tel1 == tel2);
   }
   {
      MsrpUrl tel1("msrp:+358-555-1234567;postd=pp22@foo.com;user=phone");
      MsrpUrl tel2("msrp:+358-555-1234567;POSTD=PP22@foo.com;user=phone");
      assert (tel1 != tel2);
   }
   {
      MsrpUrl tel1("tel:+358-555-1234567;postd=pp22;isub=1411");
      MsrpUrl tel2("tel:+358-555-1234567;isub=1411;postd=pp22");
      // requires us to parse the user parameters
      //assert (tel1 == tel2);
   }
   {
      MsrpUrl tel1("msrp:+358-555-1234567;postd=pp22;isub=1411@foo.com;user=phone");
      MsrpUrl tel2("msrp:+358-555-1234567;isub=1411;postd=pp22@foo.com;user=phone");
      assert (tel1 != tel2);
   }
   {
      MsrpUrl tel1("tel:+358-555-1234567;postd=pp22");
      MsrpUrl tel2("tel:+358-555-1234567;POSTD=PP22");
      MsrpUrl sip1(MsrpUrl::fromTel(tel1, MsrpUrl("msrp:foo.com")));
      MsrpUrl sip2(MsrpUrl::fromTel(tel2, MsrpUrl("msrp:foo.com")));
      assert (sip1 == sip2);
      assert (Data::from(sip1) == "msrp:+358-555-1234567;postd=pp22@foo.com;user=phone");
      assert (Data::from(sip2) == "msrp:+358-555-1234567;postd=pp22@foo.com;user=phone");
   }
   {
      MsrpUrl tel1("tel:+358-555-1234567;tsp=a.b;phone-context=5");
      MsrpUrl tel2("tel:+358-555-1234567;phone-context=5;tsp=a.b");
      MsrpUrl sip1(MsrpUrl::fromTel(tel1, MsrpUrl("msrp:foo.com")));
      MsrpUrl sip2(MsrpUrl::fromTel(tel2, MsrpUrl("msrp:foo.com")));
      assert (sip1 == sip2);
      assert (Data::from(sip1) == "msrp:+358-555-1234567;phone-context=5;tsp=a.b@foo.com;user=phone");
      assert (Data::from(sip2) == "msrp:+358-555-1234567;phone-context=5;tsp=a.b@foo.com;user=phone");
   }

   {
      MsrpUrl msrpUrl("msrp:fluffy@iii.ca:666");
      assert(msrpUrl.scheme() == "sip");
      assert(msrpUrl.user() == "fluffy");
      assert(msrpUrl.host() == "iii.ca");
      assert(msrpUrl.port() == 666);
   }
   
   {
      MsrpUrl msrpUrl("msrp:fluffy@iii.ca;transport=tcp");
      assert(msrpUrl.param(p_transport) == "tcp");
   }
   
   {
      MsrpUrl msrpUrl("msrps:fluffy@iii.ca;transport=tls");
      assert(msrpUrl.scheme() == "sips");
      assert(msrpUrl.param(p_transport) == "tls");
   }
   
   {
      MsrpUrl msrpUrl("msrp:fluffy@iii.ca;transport=sctp");
      assert(msrpUrl.param(p_transport) == "sctp");
   }
   
   {
      MsrpUrl msrpUrl("msrp:fluffy:password@iii.ca");
      assert(msrpUrl.password() == "password");
   }

   {
      MsrpUrl msrpUrl("msrp:fluffy@iii.ca;user=phone;ttl=5;lr;maddr=1.2.3.4");
      assert(msrpUrl.param(p_ttl) == 5);
      assert(msrpUrl.exists(p_lr) == true);
      assert(msrpUrl.param(p_maddr) == "1.2.3.4");
      assert(msrpUrl.param(p_user) == "phone");
   }
 
   {
      MsrpUrl msrpUrl("msrp:fluffy@iii.ca;x-fluffy=foo");
      assert(msrpUrl.exists(UnknownParameterType("x-fluffy")) == true);
      assert(msrpUrl.exists(UnknownParameterType("x-fufu")) == false);
      assert(msrpUrl.param(UnknownParameterType("x-fluffy")) == "foo");
   }
 
   {
      MsrpUrl msrpUrl("msrp:fluffy@iii.ca;method=MESSAGE");
      assert(msrpUrl.param(p_method) == "MESSAGE");
   }

   {
      MsrpUrl msrpUrl("msrp:+1(408) 444-1212:666@gw1");
      assert(msrpUrl.user() == "+1(408) 444-1212");
      assert(msrpUrl.password() == "666");
      assert(msrpUrl.host() == "gw1");
   }
 
   {
      MsrpUrl msrpUrl("msrp:fluffy;x-utag=foo@iii.ca");
      assert(msrpUrl.user() == "fluffy;x-utag=foo");
      assert(msrpUrl.host() == "iii.ca");

      Data out(Data::from(msrpUrl));
      assert(out == "msrp:fluffy;x-utag=foo@iii.ca");
   }

   {
      MsrpUrl msrpUrl("msrp:fluffy;x-utag=foo:password@iii.ca");
      assert(msrpUrl.user() == "fluffy;x-utag=foo");
      assert(msrpUrl.host() == "iii.ca");
      assert(msrpUrl.password() == "password");

      Data out(Data::from(msrpUrl));
      cerr << "!! " << out << endl;
      assert(out == "msrp:fluffy;x-utag=foo:password@iii.ca");
   }

   {
      MsrpUrl msrpUrl("tel:+14086661212");
      assert(msrpUrl.user() == "+14086661212");
      assert(msrpUrl.userParameters() == "");
      assert(msrpUrl.host() == "");
      assert(msrpUrl.password() == "");

      Data out(Data::from(msrpUrl));
      cerr << "!! " << out << endl;
      assert(out == "tel:+14086661212");
   }

   {
      MsrpUrl msrpUrl("tel:+14086661212;foo=bie");
      assert(msrpUrl.user() == "+14086661212");
      assert(msrpUrl.userParameters() == "foo=bie");
      assert(msrpUrl.host() == "");
      assert(msrpUrl.password() == "");

      Data out(Data::from(msrpUrl));
      cerr << "!! " << out << endl;
      assert(out == "tel:+14086661212;foo=bie");
   }

   {
      MsrpUrl msrpUrl("tel:+14086661212;");
      assert(msrpUrl.user() == "+14086661212");
      assert(msrpUrl.userParameters() == "");
      assert(msrpUrl.host() == "");
      assert(msrpUrl.password() == "");

      Data out(Data::from(msrpUrl));
      cerr << "!! " << out << endl;
      assert(out == "tel:+14086661212");
   }

   {
      MsrpUrl msrpUrl("msrp:;:@");
      cerr << "msrpUrl.user() = " << msrpUrl.user() << endl;
      assert(msrpUrl.user() == ";");
      assert(msrpUrl.userParameters() == "");
      assert(msrpUrl.host() == "");
      assert(msrpUrl.password() == "");

      Data out(Data::from(msrpUrl));
      cerr << "!! " << out << endl;
      assert(out == "msrp:;");
   }

   {
      MsrpUrl msrpUrl("tel:+1 (408) 555-1212");
      assert(msrpUrl.scheme() == "tel");
   }
   // Tests for user-less msrpUrls (was broken accidentally v1.44 MsrpUrl.cxx)
   {
     Data original("msrp:1.2.3.4:5060");
     Data encoded;
     MsrpUrl msrpUrl(original);

     DataStream ds(encoded);
     msrpUrl.encode(ds);
     ds.flush();
     cout << "!! original data: " << original << endl;
     cout << "!! original msrpUrl : " << msrpUrl << endl;
     cout << "!! encoded  data: " << encoded << endl;

     assert( encoded == original );
   }

   {
      // Test order irrelevance of unknown parameters
      MsrpUrl sip1("msrp:user@domain;foo=bar;baz=qux");
      MsrpUrl sip2("msrp:user@domain;baz=qux;foo=bar");
      assert (sip1 == sip2);
      assert (sip2 == sip1);
   }

   {
      // Test order irrelevance of known parameters
      MsrpUrl sip1("msrp:user@domain;ttl=15;method=foo");
      MsrpUrl sip2("msrp:user@domain;method=foo;ttl=15");

      assert (sip1 == sip2);
      assert (sip2 == sip1);
   }
   

   // tests from 3261 19.1.4
   {
      MsrpUrl sip1("msrp:alice@atlanta.com;transport=TCP");
      MsrpUrl sip2("msrp:alice@AtLanTa.CoM;Transport=tcp");

      assert(sip1 == sip2);
      assert(sip2 == sip1);
   }

   {
      MsrpUrl sip1("msrp:carol@chicago.com");
      MsrpUrl sip2("msrp:carol@chicago.com;newparam=5");
      MsrpUrl sip3("msrp:carol@chicago.com;secmsrpUrlty=on");

      assert(sip1 == sip2);
      assert(sip2 == sip1);
      assert(sip2 == sip3);
      assert(sip3 == sip2);
      assert(sip3 == sip1);
      assert(sip1 == sip3);
   }

   {
      MsrpUrl sip1("msrp:biloxi.com;transport=tcp;method=REGISTER?to=msrp:bob%40biloxi.com");
      MsrpUrl sip2("msrp:biloxi.com;method=REGISTER;transport=tcp?to=msrp:bob%40biloxi.com");

      assert(sip1 == sip2);
      assert(sip2 == sip1);
   }

  {
     MsrpUrl sip1("msrp:alice@atlanta.com?subject=project%20x&priority=urgent");
     MsrpUrl sip2("msrp:alice@atlanta.com?priority=urgent&subject=project%20x");

     assert(sip1 == sip2);
     assert(sip2 == sip1);
  }

  {
     MsrpUrl sip1("MSRP:ALICE@AtLanTa.CoM;Transport=udp"); // (different usernames)
     MsrpUrl sip2("msrp:alice@AtLanTa.CoM;Transport=UDP");

     assert(sip1 != sip2);
  }

  {
     MsrpUrl sip1("msrp:bob@biloxi.com"); // (can resolve to different ports)
     MsrpUrl sip2("msrp:bob@biloxi.com:5060");

     assert(sip1 != sip2);
  }     

  {
     MsrpUrl sip1("msrp:bob@biloxi.com"); // (can resolve to different transports)
     MsrpUrl sip2("msrp:bob@biloxi.com;transport=udp");

     assert(sip1 != sip2);
  }     

  {
     MsrpUrl sip1("msrp:bob@biloxi.com"); // (can resolve to different port and transports)
     MsrpUrl sip2("msrp:bob@biloxi.com:6000;transport=tcp");

     assert(sip1 != sip2);
  }     

  // !dlb! we ignore embedded headers at the moment
  if (false)
  {
     MsrpUrl sip1("msrp:carol@chicago.com"); // (different header component)
     MsrpUrl sip2("msrp:carol@chicago.com?Subject=next%20meeting");

     assert(sip1 != sip2);
  }     

  {
     MsrpUrl sip1("msrp:bob@phone21.boxesbybob.com"); // (even though that's what phone21.boxesbybob.com resolves to)
     MsrpUrl sip2("msrp:bob@192.0.2.4");  

     assert(sip1 != sip2);
  }

  {
    MsrpUrl sip1("msrp:carol@chicago.com");
    MsrpUrl sip2("msrp:carol@chicago.com;secmsrpUrlty=on");
    MsrpUrl sip3("msrp:carol@chicago.com;secmsrpUrlty=off");

    assert(sip1 == sip2);
    assert(sip1 == sip3);
    assert(sip2 != sip3);
    assert(sip3 != sip2);
  }

  {
    MsrpUrl sip1("msrp:carol@chicago.com");
    MsrpUrl sip2("msrp:carol@chicago.com:5060");
    MsrpUrl sip3("msrp:1.2.3.4");
    MsrpUrl sip4("msrp:1.2.3.4:5070");
    MsrpUrl sip1a("msrp:carol@chicago.com;user=phone;foo=bar");
    MsrpUrl sip2a("msrp:carol@chicago.com:5060;user=phone;foo=bar");
    MsrpUrl sip3a("msrp:1.2.3.4;user=phone;foo=bar");
    MsrpUrl sip4a("msrp:1.2.3.4:5070;user=phone;foo=bar");

    DebugLog( << "sip1.getAor==" << sip1.getAor() );
    DebugLog( << "sip1.getAorNoPort==" << sip1.getAorNoPort() );
    DebugLog( << "sip2.getAor==" << sip2.getAor() );
    DebugLog( << "sip2.getAorNoPort==" << sip2.getAorNoPort() );
    
    assert( sip1.getAor() == Data("carol@chicago.com") );
    assert( sip2.getAor() == Data("carol@chicago.com:5060") );
    assert( sip3.getAor() == Data("1.2.3.4") );
    assert( sip4.getAor() == Data("1.2.3.4:5070") );

    assert( sip1a.getAor() == Data("carol@chicago.com") );
    assert( sip2a.getAor() == Data("carol@chicago.com:5060") );
    assert( sip3a.getAor() == Data("1.2.3.4") );
    assert( sip4a.getAor() == Data("1.2.3.4:5070") );

    assert( sip1.getAorNoPort() == Data("carol@chicago.com") );
    assert( sip2.getAorNoPort() == Data("carol@chicago.com") );
    assert( sip3.getAorNoPort() == Data("1.2.3.4") );
    assert( sip4.getAorNoPort() == Data("1.2.3.4") );

    assert( sip1a.getAorNoPort() == Data("carol@chicago.com") );
    assert( sip2a.getAorNoPort() == Data("carol@chicago.com") );
    assert( sip3a.getAorNoPort() == Data("1.2.3.4") );
    assert( sip4a.getAorNoPort() == Data("1.2.3.4") );
  }
#endif
   cerr << endl << "All OK" << endl;
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

