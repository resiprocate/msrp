
namespace msrp
{

const int MAX_TRANSACTION_ID_LEN=32;


enum MsgMethod
{
   METHOD_SEND,
   METHOD_REPORT
};



class RequestMsg
{
      MsgMethod method;
      char[MAX_TRANSACTION_ID_LEN]  tranactionId;
      
};

class Uri
{
   public:
      Uri( char* string );
   private:
      char* uri;
};

   
class ByteRagne
{
      const int maxRange=MAX_INT;
      
      int start;
      int end; // maxRange for *
      int total; // maxRange for *
      
};

class Status
{
      enum namespace
      {
            MSRP // the 000 namespace 
      };
      int code;
      
      char* unkonwNamespace;
      char* unkonwnStatusCode;
      char*  textReason;
      
};

   
class RequestMsg
{
      int statusCode;
      char[MAX_TRANSACTION_ID_LEN+1]  tranactionId;
      char* phrase;

      std::vector<Uri> toPath;
      std::vector<Uri> fromPath;      
      MessageId messageId;
      SuccessReport successsReport;
      FailureReport failureReport;
      ByteRange byteRange;
      Status status;
      std::vector<char*> extHeader;

      // Mime Headers
      ContentId contentId;
      ContentDescription contentDescription;
      ContentDisposition contentDispositiion;
      MediaType mediaType;
};


class ResponseMsg
{
      int statusCode;
      char[MAX_TRANSACTION_ID_LEN+1]  tranactionId;
      MethodType method;
      char* unknownMethod;
      
      std::vector<Uri> toPath;
      std::vector<Uri> fromPath;
      MessageIdType messageId;
      SuccessReportType successsReport;
      FailureReportType failureReport;
      ByteRange byteRange;
      Status status;
      std::vector<char*> extHeader;
};


}
