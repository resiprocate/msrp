#ifndef _MSRP_SESSION
#define _MSRP_SESSION 1

#include "Constants.h"
#include "Callback.h"

namespace msrp
{
  class Session
  {
    public:

      enum
      {
        COMPLETE_MESSAGE = -1,
        TOTAL_SIZE_UNKNOWN = -2
      };

      // We need to send an offer - the offerer
      // never listens, but needs to bind to a local port.
      Session(Stack *stack
              Callback *callback, 
              std::vector<msrp::Relay*> relays
                = std::vector<msrp::Relay*>());

      // We have an offer, and need to send an answer
      Session(Stack *stack,
              Callback *callBack,
              char *toPath,
              std::vector<msrp::Relay*> relays
                = std::vector<msrp::Relay*>(),
              int listenPort = PREFERRED_PORT);

      void addRemotePath(char *toPath);
 
      int send(char *buffer, 
               int bufferLength,
               char *contentType,
               positive_report_t pr = POSITIVE_REPORT_NO,
               negative_report_t nr = NEGATIVE_REPORT_YES,
               int totalLength = COMPLETE_MESSAGE);

      void sendMore(int id,
                    char *buffer,
                    int bufferLength,
                    bool done = FALSE);

      void finish(int id);

      void abort(int id);

      void reject(int id, int status);

      void accept(int id);

      std::string getToPath() const;
      std::string getFromPath() const;

    private:
      Callback *callback;
      unsigned short localPort;
      std::vector<std::string> path;
  }
};

#endif
