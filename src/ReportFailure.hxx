#if !defined(MSRP_REPORTFAILURE_HXX)
#define MSRP_REPORTFAILURE_HXX 

#include <iosfwd>
#include "src/os/Data.hxx"
#include "src/ParserCategory.hxx"
//#include "src/ParserContainer.hxx"

namespace msrp
{


//====================
// ReportFailure:
//====================
class ReportFailure : public LazyParser //public ParserCategory
{
   public:
      //enum {commaHandling = CommasAllowedOutputCommas};
      enum Result
      {
         Yes,
         No,
         Partial
      };
      
      ReportFailure();
      ReportFailure(HeaderFieldValue* hfv, Headers::Type type);
      ReportFailure(const ReportFailure&);
      ReportFailure& operator=(const ReportFailure&);
      bool operator==(const ReportFailure& rhs) const;
      
      Result& result() const;

      virtual void parse(ParseBuffer& pb); 
      //virtual ParserCategory* clone() const;
      virtual std::ostream& encodeParsed(std::ostream& str) const;
   private:
      mutable Result  mValue;
};
 
}

#endif
