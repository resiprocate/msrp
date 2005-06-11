#ifndef _MSRP_SESSION
#define _MSRP_SESSION 1

#include "Constants.h"
#include "Callback.h"

namespace msrp
{
  class Session
  {
    public:

      // We need to send an offer - the offerer
      // never listens, but needs to bind to a local port.
      Session(Stack *stack,
              std::vector<msrp::RelayRecord*> relays
                = std::vector<msrp::RelayRecord*>());

      // We have an offer, and need to send an answer
      Session(Stack *stack,
              char *toPath,
              std::vector<msrp::RelayRecord*> relays
                = std::vector<msrp::RelayRecord*>(),
              int listenPort = PREFERRED_PORT);

      void addRemotePath(char *toPath);
 
      std::string getPath() const;

      unsigned short getLocalPort();

    private:
      bool mTheRemotePathIsSet;
      unsigned short mLocalPort;
      std::vector<std::string> mPath;
  }
};

#endif
