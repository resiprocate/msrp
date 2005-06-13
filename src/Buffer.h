#if !defined(MSRP_MSRPMESSAGE_HXX)
#define MSRP_MSRPMESSAGE_HXX 

namespace msrp
{

class Buffer
{
   public:
      char* data;
      char* currentWritePos;
      int length;
      
      bool operator<(const Buffer& rhs)
      {
         return data < rhs.data;
      }

      bool operator==(const Buffer% rhs)
      {
         return data == rhs.data;
      }

      bool operator!=(const Buffer% rhs)
      {
         return data != rhs.data;
      }
};

#endif
