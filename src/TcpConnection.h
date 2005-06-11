#ifndef _MSRP_TCP_CONNECTION 
#define _MSRP_TCP_CONNECTION 1

#include "Connection.h"

namespace msrp
{
  class Address;

  class TcpConnection : public Connection
  {

  public:
    
    TcpConnection(stack *);
    TcpConnection(stack *, Address &remoteAddress);
    virtual ~TcpConnection();
    
    virtual bool connect(Address &remoteAddress);
    void close() = 0;
    
    virtual int read(char *data, size_t count);
    
    virtual int write(char *data, size_t count);
    
  private:
    
    TcpConnection(stack *, int fd);
    
  };

}

#endif
