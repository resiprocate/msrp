#include <sys/types.h>
#include <sys/socket.h>
#include "TcpConnection.h"
#include "Address.h"
#include "Stack.h"

TcpConnection::TcpConnection(Stack& stack, const Tuple& remoteAddress)
   : mStack(stk)
{
   connect(remoteAddress);
}

TcpConnection::TcpConnection(Stack& stack, int fd)
   : mStack(stk), 
     mDescriptor(fd)
{
}

TcpConnection::~TcpConnection()
{
}

bool
TcpConnection::connect(Address &remoteAddress)
{
   if (descriptor)
   {
      return false;
   }

   switch (remoteAddress.getAddressType())
   {
      case Address::IPV4:
      {
         Ipv4Address *addr = reinterpret_cast<Ipv4Address *>(&remoteAddress);
         int status;
     
         descriptor = socket(PF_INET, SOCK_STREAM, 0);
         if (descriptor == -1)
         {
            descriptor = 0;
            return false;
         }

         status = ::connect(descriptor, *addr, sizeof(struct sockaddr_in));
         if (status == -1)
         {
            descriptor = 0;
            return false;
         }
      }
      break;

      default:
         return false;    
   }
   return true;
}


int
TcpConnection::write(char *data, size_t count)
{
   if (!descriptor) 
   {
      return Connection::FAIL;
   }

   int n = ::write(descriptor, data, count);
  
   if (n == -1) 
   {
      return Connection::FAIL;
   }

   return n;
}

int
TcpConnection::read(char *data, size_t count)
{
   if (!descriptor) 
   {
      return Connection::FAIL;
   }

   int n = ::read(descriptor, data, count);
   if (n == -1) 
   {
      return Connection::FAIL;
   }

   return n;
}
