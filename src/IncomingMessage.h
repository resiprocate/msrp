#ifndef _MSRP_INCOMINGMESSAGE
#define _MSRP_INCOMINGMESSAGE 1

#include "Constants.h"


namespace msrp
{
class Session;
class IncomingMessage
{
   public:

      IncomingMessage(Session *session, char *contentType, int minIncrementalReportFrequency = 65536);

      //reject message with statuscode
      void reject(int status);

      const char *getContentType();

   protected:       
      // Callbacks

      // In  both onDataReceived and onDataSegmentReceived buf can be ignored
      // if you are using the default allocation strategy or a copy and re-use
      // strategy of your own.
           
      //bytes were received in the incoming message, but more bytes may be
      //received contiguously into buf
      void onDataReceived(const char *startByte, 
                          int length,
                          bool finished,
                          Buffer& buf) = 0;
        
      //a contiguous segment of data was received into buf; no more bytes will
      //be written to this segment
      void onDataSegmentReceived(const char *startByte, 
                                 int length,
                                 bool finished,
                                 Buffer& buf) = 0;
        
      void onMessageAborted() = 0;
 

   private:
      Session *mSession;
      char *mContentType;
};

}

#endif
