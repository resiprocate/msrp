#ifndef _MSRP_CALLBACK
#define _MSRP_CALLBACK 1

namespace msrp
{
  class Callback
  {
    public:
      virtual void receiveData (int id,
                                char *buffer,
                                int length,
                                char *contentType,
                                int totalLength) = 0;

      virtual void receiveMoreData (int id,
                                    char *buffer,
                                    int length,
                                    bool done) = 0;

      virtual void abortData (int id, int status) = 0;

      virtual void dataProgress (int id, int bytes) {}
  }
};

#endif
