#ifndef _MSRP_SESSION
#define _MSRP_SESSION 1

#include "Constants.h"
#include "Callback.h"

namespace msrp
{
  class Session;
  class IncomingMessage
  {
    public:

      IncomingMessage(Session *session, char *contentType);

      void reject(int status, int startByte, int bufferLength);

      void accept(int startByte, int bufferLength);

      const char *getContentType();

    protected:

      // Callbacks

      onDataReceived (int startByte,
                      char *buffer, 
                      int bufferLength,
                      int totalLength, 
                      bool finished) = 0;


      onMessageAborted ();
 

    private:
      Session *mSession;
      char *mContentType;
  }
};

#endif
