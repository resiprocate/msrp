/* C++ code produced by gperf version 2.7.2 */
/* Command-line: gperf -D --enum -E -L C++ -t -k '*' --compare-strncmp -Z HeaderHash HeaderHash.gperf  */
#include <string.h>
#include <ctype.h>
#include "common/HeaderTypes.hxx"

namespace msrp
{
using namespace std;
struct headers { char *name; Headers::Type type; };
/* maximum key range = 27, duplicates = 0 */

class HeaderHash
{
private:
  static inline unsigned int hash (const char *str, unsigned int len);
public:
  static struct headers *in_word_set (const char *str, unsigned int len);
};

inline unsigned int
HeaderHash::hash (register const char *str, register unsigned int len)
{
  static unsigned char asso_values[] =
    {
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33,  0, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33,  5,  0,  0,  0,
       0, 33, 33,  0, 33, 33, 10, 10, 33, 33,
       0, 33,  0,  0,  0, 33,  0, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33,  0, 33,  0,
       0,  0,  0,  0,  0,  0, 33, 33, 15,  0,
       0,  0,  0, 33,  0,  0,  0,  0, 33, 33,
      33,  0, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
      33, 33, 33, 33, 33, 33
    };
  register int hval = len;

  switch (hval)
    {
      default:
      case 25:
        hval += asso_values[(unsigned char)str[24]];
      case 24:
        hval += asso_values[(unsigned char)str[23]];
      case 23:
        hval += asso_values[(unsigned char)str[22]];
      case 22:
        hval += asso_values[(unsigned char)str[21]];
      case 21:
        hval += asso_values[(unsigned char)str[20]];
      case 20:
        hval += asso_values[(unsigned char)str[19]];
      case 19:
        hval += asso_values[(unsigned char)str[18]];
      case 18:
        hval += asso_values[(unsigned char)str[17]];
      case 17:
        hval += asso_values[(unsigned char)str[16]];
      case 16:
        hval += asso_values[(unsigned char)str[15]];
      case 15:
        hval += asso_values[(unsigned char)str[14]];
      case 14:
        hval += asso_values[(unsigned char)str[13]];
      case 13:
        hval += asso_values[(unsigned char)str[12]];
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

struct headers *
HeaderHash::in_word_set (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 15,
      MIN_WORD_LENGTH = 6,
      MAX_WORD_LENGTH = 25,
      MIN_HASH_VALUE = 6,
      MAX_HASH_VALUE = 32
    };

  static struct headers wordlist[] =
    {
      {"Status", Headers::Status},
      {"To-Path", Headers::ToPath},
      {"From-Path", Headers::FromPath},
      {"Content-ID", Headers::ContentId},
      {"Content-Type", Headers::ContentType},
      {"Report-Success", Headers::ReportSuccess},
      {"Byte-Range", Headers::ByteRange},
      {"Content-Encoding", Headers::ContentEncoding},
      {"Content-Disposition", Headers::ContentDisposition},
      {"Message-ID", Headers::MessageId},
      {"Content-Length", Headers::ContentLength},
      {"Content-Transfer-Encoding", Headers::ContentTransferEncoding},
      {"Content-Language", Headers::ContentLanguage},
      {"Report-Failure", Headers::ReportFailure},
      {"MIME-Version", Headers::MIMEVersion}
    };

  static signed char lookup[] =
    {
      -1, -1, -1, -1, -1, -1,  0,  1, -1,  2,  3, -1,  4, -1,
       5,  6,  7, -1, -1,  8,  9, -1, -1, -1, 10, 11, 12, -1,
      -1, 13, -1, -1, 14
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist[index].name;

              if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
                return &wordlist[index];
            }
        }
    }
  return 0;
}
}
