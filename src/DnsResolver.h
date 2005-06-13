#if !defined(MSRP_DnsResolver_hxx)
#define MSRP_DnsResolver_hxx


#include "resiprocate/os/Tuple.hxx"

namespace msrp
{

class DnsResultHandler
{
   public:
      virtual void onResolved(const Data& name, const resip::Tuple& tuple) const=0;
};

class DnsResolver
{
   public:
      DnsResolver(DnsResultHandler& resultH) : mResultH(resultH)
      {
      }
      
      void resolve(const Data& name);
      void blacklist(const Data& name, const Tuple& tuple);
      
   private:
      DnsResultHandler& mResultH;
};


}

#endif
