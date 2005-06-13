#ifndef _MSRP_TCPLISTENER
#define _MSRP_TCPLISTENER 1

#include <sys/types.h>
#include "Listener.h"

namespace msrp
{

class TcpListener : public Listener
{
   public:
      TcpListener(Stack& stk, unsigned short port);
      virtual ~TcpListener();
      
   protected:
      virtual Connection* make(sockaddr &aSockaddr);
};

}
#endif
