#include "msrp_address.h"
#include <iostream>

msrp::Ipv4Address::Ipv4Address(const struct sockaddr_in &address)
{
  addr = address;
}

msrp::Ipv4Address::Ipv4Address(const struct in_addr &address, 
                               unsigned short port)
{
  addr.sin_family = AF_INET;
  addr.sin_addr = address;
  addr.sin_port = htons(port);
}

msrp::Ipv4Address::Ipv4Address(const std::string &name, unsigned short port)
{
  struct in_addr address;
  address.s_addr = inet_addr(name.c_str());

  addr.sin_family = AF_INET;
  addr.sin_addr = address;
  addr.sin_port = htons(port);
}

msrp::Ipv4Address::Ipv4Address(const std::string &namePort)
{
  std::string::size_type colon = namePort.find_last_of(':');

  unsigned short port = atoi(namePort.substr(colon+1).c_str());
  std::string name = namePort.substr(0,colon);

  struct in_addr address;
  address.s_addr = inet_addr(name.c_str());

  addr.sin_family = AF_INET;
  addr.sin_addr = address;
  addr.sin_port = htons(port);
}


void
msrp::Ipv4Address::dump(std::ostream &os) const
{
  const unsigned char *address = reinterpret_cast<const unsigned char *>
                                   (&addr.sin_addr);

  os << (unsigned int)(address[0]) << '.';
  os << (unsigned int)(address[1]) << '.';
  os << (unsigned int)(address[2]) << '.';
  os << (unsigned int)(address[3]) << ':';
  os << ntohs(addr.sin_port);
}
