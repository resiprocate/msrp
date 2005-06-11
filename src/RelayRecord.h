#ifndef _MSRP_RELAY
#define _MSRP_RELAY 1

#include <string>

namespace msrp
{
  class RelayRecord
  {

    public:
      RelayRecord(std::string relay,
            std::string user,
            std::string pass) : 
        relayName(relay), userName(user), password(pass) {}

      const std::string &getRelayRecordName() const {return relayName;}
      const std::string &getUserName()  const {return userName;}
      const std::string &getPassword()  const {return password;}

    private:
      std::string relayName;
      std::string userName;
      std::string password;
  }
}

#endif
