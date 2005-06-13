#include <assert.h>
#include <iostream>

#include "DnsResolver.h"

// TODO: this is a placeholder and must be clobbered with a real
// implementation

void 
msrp::DnsResolver::resolve(const Data& name)
{

    if (!resip::DnsUtil::isIpAddress(name))
    {
        std::cerr << __FILE__ << " line " << __LINE__ << std::endl;
        assert(0);
    }
    
    resip::Tuple tup(name,0,resip::TransportType::UNKNOWN_TRANSPORT);

    mResultH.onResolved(name, tup);



}

