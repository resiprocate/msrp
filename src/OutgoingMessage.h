#ifndef _MSRP_SESSION
#define _MSRP_SESSION 1

#include "Constants.h"
#include "Callback.h"

namespace msrp
{
class Session;

enum
{
   COMPLETE_MESSAGE = -1,
   TOTAL_SIZE_UNKNOWN = -2
};

class OutgoingMessage
{
   public:
      OutgoingMessage(Session *session,
                      char *contentType,
                      positive_report_t pr = POSITIVE_REPORT_NO,
                      negative_report_t nr = NEGATIVE_REPORT_YES);

      // To start transmission, call "send" 
      void send(int startByte,
                char *buffer, 
                int bufferLength,
                bool done = false,
                int totalLength = TOTAL_SIZE_UNKNOWN);
        
      void finish();

      void abort();


   protected:
      // Callbacks

      /// The stack needs more data to send
      virtual void onRequestData() = 0;

      /// The stack has sent all the bytes it knows about
      virtual void onSent(char *buffer, int length) = 0;

      /// The stack has received indication that the remote party has aborted
      virtual void onReject(int statusCode, int startByte, int length) = 0;

      /// We have received an incremental confirmation
      virtual void onProgress(int startByte, int length) = 0;
 
   private:
      // OutgoingMessage::transmit will 
      enum TransmitResult
      {
         MoreData,
         Done,
         Failed
      };

      // Session calls transmit. 
      // ?dcm? make composite?
      TransmitResult transmit()
      {
         //mSession->mConnection->transmit(...);
      }
      

      Session *mSession;
}
};

#endif
