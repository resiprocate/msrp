#if !defined(MSRP_BYTERANGE_HXX)
#define MSRP_BYTERANGE_HXX 

#include <iosfwd>
#include "common/os/Data.hxx"
#include "common/ParserCategory.hxx"

namespace msrp
{


//====================
// ByteRange:
//====================
class ByteRange : public ParserCategory
{
   public:
      enum {commaHandling = CommasAllowedOutputCommas};

      ByteRange();
      ByteRange(HeaderFieldValue* hfv, Headers::Type type);
      ByteRange(const ByteRange&);
      ByteRange& operator=(const ByteRange&);
      bool operator==(const ByteRange& rhs) const;
      bool operator<(const ByteRange& rhs) const;

      bool& endInt() const;
      bool& totalInt() const;

      int& end() const;
      int& total() const;      
      int& start() const;

      virtual void parse(ParseBuffer& pb);
      virtual ParserCategory* clone() const;
      virtual std::ostream& encodeParsed(std::ostream& str) const;
   private:
      mutable int  mStart;
      mutable int mEnd;
      mutable int mTotal;
      mutable bool mEndInt;
      mutable bool mTotalInt;
};
 
}

#endif
