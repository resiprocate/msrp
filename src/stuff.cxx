/*

namespace msrp
{
  
  
  class Uri
  {
    public:
      Uri( char* string );
    private:
      char* uri;
  };
  
     
  class ByteRange
  {
    private:
      const int maxRange=MAX_INT;
        
      int start;
      int end; // maxRange for *
      int total; // maxRange for *
  };
  
  class Status
  {
    public:
      enum namespace
      {
        MSRP = 0// the 000 namespace 
      };
  
    private:
      int code;
        
      char* unknownNamespace;
      char* unknownStatusCode;
      char* textReason;
  };
  
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
  
}
*/
