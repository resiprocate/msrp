#ifndef _MSRP_STACK
#define _MSRP_STACK 1

#include <map>

#include "Constants.h"
#include "Callback.h"
#include "Session.h"

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
