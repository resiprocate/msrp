#include "TcpListener.h"
#include "Address.h"
#include "Stack.h"

using namespace msrp;

TcpListener::TcpListener(Stack& stack, unsigned short port) : Listener(stack, port)
{
}

Connection*
TcpListener::make(sockaddr &aSockaddr)
{
    Tuple tup(aSockaddr,resip::TransportType::TCP);
    
    // The expectation is that connection makes a copy of this, and
    // that it is passed as const ref
    return new TcpConnection(mStack, tup);

}

