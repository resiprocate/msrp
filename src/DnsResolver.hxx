#if !defined(MSRP_DnsResolver_hxx)
#define MSRP_DnsResolver_hxx

namespace msrp
{

class DnsResult
{
   public:
      virtual void onResolved(const Data& name, const resip::Tuple& tuple) const=0;
};

class DnsResolver
{
   public:
      DnsResolver(DnsResult& result) : mResult(result)
      {
      }
      
      void resolve(const Data& name);
      void blacklist(const Data& name, const Tuple& tuple);
      
   private:
      DnsResult& mResult;
};


}

#endif
