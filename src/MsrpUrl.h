#ifndef MsrpUrl_hxx
#define MsrpUrl_hxx

#include "common/ParserCategory.hxx"

namespace msrp
{

class MsrpUrl : public ParserCategory
{
   public:
      MsrpUrl();
      MsrpUrl(const MsrpUrl&);
      explicit MsrpUrl(const Data& data);

      ~MsrpUrl();
      
      Data& host() {checkParsed(); return mHost;}
      const Data& host() const {checkParsed(); return mHost;}
      Data& user() {checkParsed(); return mUser;}
      const Data& user() const {checkParsed(); return mUser;}

      Data& scheme() {checkParsed(); return mScheme;}
      const Data& scheme() const {checkParsed(); return mScheme;}
      int& port() {checkParsed(); return mPort;}
      int port() const {checkParsed(); return mPort;}

      Data& sessionId() { checkParsed(); return mSessionId; }
      const Data& sessionId() const { checkParsed(); return mSessionId; }

      Data& transport() { checkParsed(); return mTransport; }
      const Data& transport() const { checkParsed(); return mTransport; }

      virtual void parse(ParseBuffer& pb);
      virtual ParserCategory* clone() const;
      virtual std::ostream& encodeParsed(std::ostream& str) const;   
   
      MsrpUrl& operator=(const MsrpUrl& rhs);
      bool operator==(const MsrpUrl& other) const;
      bool operator!=(const MsrpUrl& other) const;
      bool operator<(const MsrpUrl& other) const;

   protected:
      mutable Data mScheme;
      mutable Data mHost;
      mutable Data mUser;
      mutable int mPort;
      mutable Data mSessionId;
      mutable Data mTransport;

      // cache for IPV6 host comparison
      mutable Data mCanonicalHost;
};

}

#include "common/os/HashMap.hxx"

#if  defined(__INTEL_COMPILER )
namespace std
{
size_t hash_value(const msrp::MsrpUrl& v);
}

#elif defined(HASH_MAP_NAMESPACE)  //#elif ( (__GNUC__ == 3) && (__GNUC_MINOR__ >= 1) )
namespace HASH_MAP_NAMESPACE
{

template <>
struct hash<msrp::MsrpUrl>
{
      size_t operator()(const msrp::MsrpUrl& v) const;
};

}
#endif


#endif
