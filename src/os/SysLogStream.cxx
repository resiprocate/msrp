#include "src/os/SysLogBuf.hxx"
#include "src/os/SysLogStream.hxx"

using msrp::SysLogStream;
using msrp::SysLogBuf;

// Remove warning about 'this' use in initiator list - pointer is only stored
#if defined(WIN32)
#pragma warning( disable : 4355 ) // using this in base member initializer list 
#endif

SysLogStream::SysLogStream() :
   std::ostream (this)
{
}

SysLogStream::~SysLogStream()
{
}
