#if !defined(MSRP_CONTENTS_HXX)
#define MSRP_CONTENTS_HXX 

#include <iosfwd>

#include "common/LazyParser.hxx"
#include "common/Mime.hxx"
#include "common/Token.hxx"
#include "common/StringCategory.hxx"
#include "common/Headers.hxx"
#include "common/HeaderFieldValue.hxx"
#include "common/os/Data.hxx"
#include "common/os/HashMap.hxx"

namespace msrp
{

class Contents;
class HeaderFieldValue;
class ParseBuffer;

class ContentsFactoryBase
{
   public:
      virtual Contents* create(HeaderFieldValue* hfv, 
                               const Mime& contentType) const = 0;
      virtual Contents* convert(Contents* c) const = 0;
};

// MIME header types
class MIME_Header
{
};

class H_ContentID : public MIME_Header
{
   public:
      typedef Token Type;
};
extern H_ContentID h_ContentID;

class H_ContentDescription : public MIME_Header
{
   public:
      typedef StringCategory Type;
};
extern H_ContentDescription h_ContentDescription;
      
// Common type for all body contents
class Contents : public LazyParser
{
   public:
      // pass Mime instance for parameters

      Contents(HeaderFieldValue* headerFieldValue, const Mime& contentsType);
      Contents(const Mime& contentsType);
      Contents(const Contents& rhs);
      virtual ~Contents();
      Contents& operator=(const Contents& rhs);

      void preParseHeaders(ParseBuffer& pb);
      std::ostream& encodeHeaders(std::ostream& str) const;

      // access to wrapped contents (signed, encrypted)
      virtual Contents* getContents() {return this;}

      Contents* getContents(const Mime&);

      virtual Data getBodyData() const;

      virtual Contents* clone() const = 0;
      const Mime& getType() const {return mType;}

      static Contents* createContents(const Mime& contentType, 
                                      const Data& contents);

      bool exists(const HeaderBase& headerType) const;
      void remove(const HeaderBase& headerType);

      bool exists(const MIME_Header& headerType) const;
      void remove(const MIME_Header& headerType);

      // shared header types
      H_ContentType::Type& header(const H_ContentType& headerType) const;
      H_ContentDisposition::Type& header(const H_ContentDisposition& headerType) const;
      H_ContentTransferEncoding::Type& header(const H_ContentTransferEncoding& headerType) const;
      H_ContentLanguages::Type& header(const H_ContentLanguages& headerType) const;

      // MIME specific header types
      H_ContentID::Type& header(const H_ContentID& headerType) const;
      H_ContentDescription::Type& header(const H_ContentDescription& headerType) const;

      int& verion() {return mVersion;}
      int& minorVersion() {return mMinorVersion;}

      static HashMap<Mime, ContentsFactoryBase*>& getFactoryMap();

   protected:
      void clear();
      virtual const Data& errorContext() const;

      mutable Mime mType;
      mutable H_ContentDisposition::Type *mDisposition;
      mutable H_ContentTransferEncoding::Type *mTransferEncoding;      
      mutable H_ContentLanguages::Type *mLanguages;
      mutable Token *mId;
      mutable H_ContentDescription::Type *mDescription;
      mutable StringCategory *mLength;

      mutable int mVersion;
      mutable int mMinorVersion;

   private:
      static HashMap<Mime, ContentsFactoryBase*>* FactoryMap;
};

template<class T>
class ContentsFactory : public ContentsFactoryBase
{
   public:
      ContentsFactory()
      {
         HeaderFieldValue hfv;
         assert(Contents::getFactoryMap().count(T::getStaticType()) == 0);
         Contents::getFactoryMap()[T::getStaticType()] = this;
      }

      explicit ContentsFactory(const Mime& nonStandardType)
      {
         HeaderFieldValue hfv;
         Contents::getFactoryMap()[nonStandardType] = this;
      }

      virtual ~ContentsFactory()
      {
         HashMap<Mime, ContentsFactoryBase*>::iterator i;
         i = Contents::getFactoryMap().find(T::getStaticType());
         Contents::getFactoryMap().erase(i);
         if (Contents::getFactoryMap().size() == 0)
         {
            delete &Contents::getFactoryMap();
         }
      }
      
      // pass Mime instance for parameters
      virtual Contents* create(HeaderFieldValue* hfv, const Mime& contentType) const
      {
         return new T(hfv, contentType);
      }

      virtual Contents* convert(Contents* c) const
      {
         return dynamic_cast<T*>(c);
      }
};

}

#endif

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this std::list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this std::list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */
