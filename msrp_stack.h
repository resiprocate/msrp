#ifndef _MSRP_STACK
#define _MSRP_STACK 1

#include <map>

#include "msrp_constants.h"
#include "msrp_callback.h"
#include "msrp_session.h"

namespace msrp
{
  class Stack
  {
    public:
      Stack(char *localName);

    private:
      std::map<unsigned short, Socket> sockets;
  }
};

#endif
