#include <iostream>
#include <sstream>
#include "msrp_address.h"

int main(int argc, char **argv)
{
  struct in_addr localhost;

  localhost.s_addr = htonl(0x7f000001);

  msrp::Ipv4Address addr1("192.168.0.12:5060");
  msrp::Ipv4Address addr2("192.168.0.20",1234);
  msrp::Ipv4Address addr3(localhost,2);

  std::ostringstream buf;

  std::cout << addr1 << std::endl;
  buf << addr1;
  if (buf.str() != "192.168.0.12:5060")
  {
    std::cerr << "Failed at line " << __LINE__ << std::endl;
    exit (-1);
  }
  buf.str("");
  
  std::cout << addr2 << std::endl;
  buf << addr2;
  if (buf.str() != "192.168.0.20:1234")
  {
    std::cerr << "Failed at line " << __LINE__ << std::endl;
    exit (-1);
  }
  buf.str("");
 
  std::cout << addr3 << std::endl;
  buf << addr3;
  if (buf.str() != "127.0.0.1:2")
  {
    std::cerr << "Failed at line " << __LINE__ << std::endl;
    exit (-1);
  }
  buf.str("");


  return 0;
}
