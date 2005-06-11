#include "Listener.h"
#include "Address.h"
#include "Stack.h"

msrp::Listener::Listener(Stack *stk)
  : mLocalAddress(0), mStack(stk), mDescriptor(0)
{}

msrp::Listener::~Listener()
{}
