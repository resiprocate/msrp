#include "Listener.h"
#include "Address.h"
#include "Stack.h"

using namespace msrp;

Listener::Listener(Stack& stack, resip::IpVersion version, unsigned short listenPort)
   : mStack(stack),
     mDescriptor(-1),
     mPort(listenPort),
     mBacklog(64)
{
#ifdef USE_IPV6
   mDescriptor = ::socket(version == V4 ? PF_INET : PF_INET6, SOCK_STREAM, 0);
#else
   assert(version == V4);
   mDescriptor = ::socket(PF_INET, SOCK_STREAM, 0);
#endif

   if ( fd == INVALID_SOCKET )
   {
      int e = getErrno();
      InfoLog (<< "Failed to create socket: " << strerror(e));
      throw Exception("Can't create Listener", __FILE__,__LINE__);
   }

#if !defined(WIN32)
   int on = 1;
   if ( ::setsockopt ( mDescriptor, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) )
   {
	   int e = getErrno();
       InfoLog (<< "Couldn't set sockoptions SO_REUSEPORT | SO_REUSEADDR: " << strerror(e));
       error(e);
       throw Exception("Failed setsockopt", __FILE__,__LINE__);
   }
#endif

   makeSocketNonBlocking(mDescriptor);

   // do the listen, seting the maximum queue size for compeletly established
   // sockets -- on linux, tcp_max_syn_backlog should be used for the incomplete
   // queue size(see man listen)
   int e = listen(mFd, mBacklog );

   if (e != 0 )
   {
      int e = getErrno();
      InfoLog (<< "Failed listen " << strerror(e));
      error(e);
      // !cj! deal with errors
	  throw Exception("Address already in use", __FILE__,__LINE__);
   }

}

Listener::~Listener()
{
   ::close(mDescriptor);
}

Connection* 
Listener::accept()
{
   struct sockaddr_in from;
   int fromlen = sizeof(from); 
   int result = accept(mDescriptor, (struct sockaddr *) &from, &fromlen);
   if (result == -1)
   {
      // !jf! may want to throw an Exception here
      return 0;
   }

   return make((struct sockaddr*)&from );
}


