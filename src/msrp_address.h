#ifndef _MSRP_ADDRESS
#define _MSRP_ADDRESS 1

#include <string>
#include <ostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

namespace msrp
{
  class Address
  {
    public:
      typedef enum
      {
        IPV4
      } address_t;
 
      address_t getAddressType() const { return addressType; }
      virtual void dump(std::ostream &os) const = 0;

    protected:
      Address() {;}
      address_t addressType;
  };


  class Ipv4Address: public Address
  {
    public:
      Ipv4Address(const std::string &namePort);
      Ipv4Address(const std::string &name, unsigned short port);
      Ipv4Address(const struct in_addr &address, unsigned short port);
      Ipv4Address(const struct sockaddr_in &address);

      operator const struct sockaddr*()    const;
      operator const struct sockaddr_in*() const;
      operator const struct in_addr*()     const;

      virtual void dump(std::ostream &os) const;

    private:
      sockaddr_in addr;
  };

  // XXX IPv6 Address Class TBD
}

inline
msrp::Ipv4Address::operator const struct sockaddr*() const
{
  return reinterpret_cast<const struct sockaddr*>(&(addr));
}

inline
msrp::Ipv4Address::operator const struct sockaddr_in*() const
{
  return &(addr);
}

inline
msrp::Ipv4Address::operator const struct in_addr*() const
{
  return &(addr.sin_addr);
}

inline std::ostream &
operator<< (std::ostream &os, const msrp::Address &address)
{
  address.dump(os);
  return os;
}

#endif