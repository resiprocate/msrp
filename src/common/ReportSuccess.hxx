#if !defined(MSRP_REPORTSUCCESS_HXX)
#define MSRP_REPORTSUCCESS_HXX 

#include <iosfwd>
#include "common/os/Data.hxx"
#include "common/ParserCategory.hxx"
//#include "common/ParserContainer.hxx"

namespace msrp
{


//====================
// ReportSuccess:
//====================
class ReportSuccess : public ParserCategory
{
   public:
      enum {commaHandling = CommasAllowedOutputCommas};

      ReportSuccess();
      ReportSuccess(HeaderFieldValue* hfv, Headers::Type type);
      ReportSuccess(const ReportSuccess&);
      ReportSuccess& operator=(const ReportSuccess&);
      bool operator==(const ReportSuccess& rhs) const;
 
      bool& success() const;

      virtual void parse(ParseBuffer& pb); 
      virtual ParserCategory* clone() const;
      virtual std::ostream& encodeParsed(std::ostream& str) const;
   private:
      mutable bool mValue;
};
 
}

#endif
