#include "OutgoingMessage.h"

msrp::OutgoingMessage::OutgoingMessage(
  msrp::Session *session,
  char *contentType,
  msrp::positive_report_t pr,
  msrp::negative_report_t nr)
  : mSession(session), 
    mPositiveReports(pr),
    mNegativeReports(nr),
    mRoar()
{
  int ctlen = strlen(contentType);
  mContentType = new char[ctlen + 1];
  memmove(mContentType, contentType, ctlen);
}


msrp::OutgoingMessage::~OutgoingMessage()
{
  delete[](mContentType);
}

// Called by the user
void
msrp::OutgoingMessage::send(
  int startByte,
  char *buffer,
  int bufferLength,
  bool done,
  int totalLength)
{
  // Magic goes here
}

// Called by the Session
msrp::Transmit::Result
msrp::OutgoingMessage::transmit()
{
  int bytesWritten = 
    mSession->mConnection->transmit(mRoar,
                                    mBuffer + mBufferOffset,
                                    bufferSize - mBufferOffset,
                             mDone ? Connection::DONE : Connection::CONTINUE);

  if (bytesWritten < 0)
  {
    return Failed;
  }
  if (bytesWritten == (mBufferSize - mBufferOffset))
  {
    return Done;
  }
  else
  {
    mBufferOffset += bytesWritten;
    return MoreData;
  }
}
