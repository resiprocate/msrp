#ifndef _MSRP_TRANSPORT_GROUP
#define _MSRP_TRANSPORT_GROUP 1

namespace msrp
{
  class TransportGroup
  {
    public:
      TransportGroup();
      ~TransportGroup();

      void addMember(Transport &);
      void removeMember(Transport &);
    
      int read(unsigned char *message, size_t count,
               Transport *& member);
    private:
      std::map<int,Transport *> members;
      std::map<int,Transport *>::iterator nextMember;
  };
}

#endif
