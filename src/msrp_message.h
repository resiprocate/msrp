  
namespace msrp
{

  class Message
  {
    public:
      const int MAX_TRANSACTION_ID_LEN=32;
  
      enum MessageMethod
      {
         METHOD_SEND,
         METHOD_REPORT
      };
  
    private:
      char[MAX_TRANSACTION_ID_LEN+1]  tranactionId;
      std::vector<Uri> toPath;
      std::vector<Uri> fromPath;    
        
  };
  
     
  class RequestMessage : public Message
  {
    private:
      char* phrase;
  
      MethodType method;
      char* unknownMethod;
  
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
  
  
  class ResponseMessage : public Message
  {
    private:
      int statusCode;
      
      MessageIdType messageId;
      SuccessReportType successsReport;
      FailureReportType failureReport;
      ByteRange byteRange;
      Status status;
      std::vector<char*> extHeader;
  };
  
  
}
