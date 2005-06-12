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
                      int totalLength = TOTAL_SIZE_UNKNOWN,
                      positive_report_t pr = POSITIVE_REPORT_NO,
                      negative_report_t nr = NEGATIVE_REPORT_YES);

      virtual ~OutgoingMessage();

      // To start transmission, call "send" 
      void send(int startByte,
                char *buffer, 
                int bufferLength,
                bool done = false);
        
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
      // Session calls transmit. 
      // ?dcm? make composite?
      Transmit::Result transmit();

      Session *mSession;
      char *mContentType;
      int mTotalLength;
      positive_report_t mPositiveReports;
      negative_report_t mNegativeReports;
      MsrpRoar mRoar;

      char *mBuffer;
      int mBufferLength;
      int mBufferOffset;
      int mCurrByte;
      bool mDone;
}
};

#endif
