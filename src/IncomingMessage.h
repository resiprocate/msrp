#ifndef _MSRP_INCOMINGMESSAGE
#define _MSRP_INCOMINGMESSAGE 1

#include "Constants.h"


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

      void onDataReceived (int startByte,
                      char *buffer, 
                      int bufferLength,
                      int totalLength, 
                      bool finished);


      void onMessageAborted ();
 

    private:
      Session *mSession;
      char *mContentType;
  };

}

#endif
