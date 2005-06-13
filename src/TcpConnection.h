#ifndef _MSRP_TCP_CONNECTION 
#define _MSRP_TCP_CONNECTION 1

#include "msrp/Connection.h"

namespace msrp
{
class Tuple;

class TcpConnection : public Connection
{
   public:
      TcpConnection(Stack&, const Tuple& remoteTuple);
      TcpConnection(Stack&, int fd);
      virtual ~TcpConnection();
    
      virtual bool connect(const Tuple& remoteTuple);
      void close() = 0;
    
      virtual int read(char *data, size_t count);
      virtual int write(char *data, size_t count);
    
   private:
    
      TcpConnection(Stack *, int fd);
    
};

}

#endif
