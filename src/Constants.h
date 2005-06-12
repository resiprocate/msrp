#ifndef _MSRP_CONSTANTS
#define _MSRP_CONSTANTS 1

namespace msrp
{
  typedef enum
  {
    TRANSACTION_OK           = 200,
    UNINTELLIGIBLE           = 400,
    NOT_ALLOWED              = 403,
    TIMEOUT                  = 400,
    STOP_SENDING_MESSAGE     = 413,
    MIME_TYPE_NOT_UNDERSTOOD = 415,
    PARAMETER_OUT_OF_BOUNDS  = 423,
    MUST_USE_TLS             = 426,
    SESSION_DOES_NOT_EXIST   = 481,
    UNKNOWN_METHOD           = 501,
    SESSION_ALREADY_EXISTS   = 506
  } status_t;

  enum
  {
    PREFERRED_PORT = 7534
  };

  typedef enum
  {
    POSITIVE_REPORT_YES,
    POSITIVE_REPORT_NO,
    POSITIVE_REPORT_INCREMENTAL,
  } positive_report_t;

  typedef enum
  {
    NEGATIVE_REPORT_YES,
    NEGATIVE_REPORT_NO,
    NEGATIVE_REPORT_PARTIAL,
  } negative_report_t;

struct Transmit
{
      
      typedef enum 
      {
         MoreData,
         Done,
         Failed
      } Result;
      
};

enum
{
   COMPLETE_MESSAGE = -1,
   TOTAL_SIZE_UNKNOWN = -2
} message_size_t;

};

#endif
