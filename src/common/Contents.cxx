#if defined(HAVE_CONFIG_H)
#include "common/config.hxx"
#endif

#include "common/Contents.hxx"
#include "common/os/ParseBuffer.hxx"
#include "common/os/Logger.hxx"
#include "common/GenericContents.hxx"
#include "common/os/WinLeakCheck.hxx"

using namespace msrp;

#define RESIPROCATE_SUBSYSTEM Subsystem::CONTENTS

H_ContentID msrp::h_ContentID;
H_ContentDescription msrp::h_ContentDescription;

HashMap<Mime, ContentsFactoryBase*>* Contents::FactoryMap = 0;

Contents::Contents(HeaderFieldValue* headerFieldValue,
                   const Mime& contentType) 
   : LazyParser(headerFieldValue),
     mType(contentType),
     mDisposition(0),
     mTransferEncoding(0),
     mLanguages(0),
     mId(0),
     mDescription(0),
	 mLength(0),
     mVersion(1),
     mMinorVersion(0)
{}

Contents::Contents(const Mime& contentType) 
   : mType(contentType),
     mDisposition(0),
     mTransferEncoding(0),
     mLanguages(0),
     mId(0),
     mDescription(0),
	 mLength(0),
     mVersion(1),
     mMinorVersion(0)
{}

Contents::Contents(const Contents& rhs) 
    : LazyParser(rhs),
      mType(rhs.mType),
      mDisposition(0),
      mTransferEncoding(0),
      mLanguages(0),
      mId(0),
      mDescription(0),
	  mLength(0),
      mVersion(1),
      mMinorVersion(0)
{
   *this = rhs;
}

Contents::~Contents()
{
   clear();
}

void
Contents::clear()
{
   delete mDisposition;
   delete mTransferEncoding;
   delete mLanguages;
   delete mId;
   delete mDescription;

   mDisposition = 0;
   mTransferEncoding = 0;
   mLanguages = 0;
   mId = 0;
   mDescription = 0;
   mLength = 0;
}

const Data&
Contents::errorContext() const
{
   static const Data context("Contents");
   return context;
}

Contents& 
Contents::operator=(const Contents& rhs) 
{
   if (this != &rhs)
   {
      LazyParser::operator=(rhs); 
      mType = rhs.mType;
      if (rhs.mDisposition)
      {
          if (mDisposition) delete mDisposition;
          mDisposition = new H_ContentDisposition::Type(*rhs.mDisposition);
      }
      if (rhs.mTransferEncoding)
      {
          if (mTransferEncoding) delete mTransferEncoding;
          mTransferEncoding = new H_ContentTransferEncoding::Type(*rhs.mTransferEncoding);
      }
      if (rhs.mLanguages)
      {
          if (mLanguages) delete mLanguages;
          mLanguages = new H_ContentLanguages::Type(*rhs.mLanguages);
      }
      if (rhs.mId)
      {
          if (mId) delete mId;
          mId = new Token(*rhs.mId);
      }
      if (rhs.mDescription)
      {
          if (mDescription) delete mDescription;
          mDescription = new StringCategory(*rhs.mDescription);
      }

      mVersion = rhs.mVersion;
      mMinorVersion = rhs.mMinorVersion;
   }

   return *this;
}

HashMap<Mime, ContentsFactoryBase*>& 
Contents::getFactoryMap()
{
   if (Contents::FactoryMap == 0)
   {
      Contents::FactoryMap = new HashMap<Mime, ContentsFactoryBase*>();
   }
   return *Contents::FactoryMap;
}

Contents*
Contents::getContents(const Mime& m)
{
   if (Contents::getFactoryMap().find(m) != Contents::getFactoryMap().end())
   {
      return Contents::getFactoryMap()[m]->convert(getContents());
   }
   else
   {
      // return a generic contents and hope for the best
      return new GenericContents(mHeaderField, m);
   }
}

Contents*
Contents::createContents(const Mime& contentType, 
                         const Data& contents)
{
  // !ass! why are we asserting that the Data doesn't own the buffer?
  // .dlb. because this method is to be called only within a multipart
  // !ass! HFV is an overlay -- then setting c->mIsMine to true ?? dlb Q
  // .dlb. we are telling the content that it owns its HFV, not the data that it
  // .dlb. owns its memory

   assert(!contents.mMine);
   HeaderFieldValue *hfv = new HeaderFieldValue(contents.data(), contents.size());

   Contents* c;
   if (Contents::getFactoryMap().find(contentType) != Contents::getFactoryMap().end())
   {
      c = Contents::getFactoryMap()[contentType]->create(hfv, contentType);
   }
   else
   {
      c = new GenericContents(hfv, contentType);
   }
   c->mIsMine = true;
   return c;
}

bool
Contents::exists(const HeaderBase& headerType) const
{
   checkParsed();
   switch (headerType.getTypeNum())
   {
      case Headers::ContentType :
      {
         return true;
      }
      case Headers::ContentDisposition :
      {
         return mDisposition != 0;
      }
      case Headers::ContentTransferEncoding :
      {
         return mTransferEncoding != 0;
      }
      case Headers::ContentLanguage :
      {
         return mLanguages != 0;
      }
      default : return false;
   }
}

bool
Contents::exists(const MIME_Header& type) const
{
   if (&type == &h_ContentID)
   {
      return mId != 0;
   }
   
   if (&type == &h_ContentDescription)
   {
      return mDescription != 0;
   }

   assert(false);
   return false;
}

void
Contents::remove(const HeaderBase& headerType)
{
   switch (headerType.getTypeNum())
   {
      case Headers::ContentDisposition :
      {
         delete mDisposition;
         mDisposition = 0;
         break;
      }
      case Headers::ContentLanguage :
      {
         delete mLanguages;
         mLanguages = 0;
         break;
      }
      case Headers::ContentTransferEncoding :
      {
         delete mTransferEncoding;
         mTransferEncoding = 0;
         break;
      }
      default :
         ;
   }
}

void
Contents::remove(const MIME_Header& type)
{
   if (&type == &h_ContentID)
   {
      delete mId;
      mId = 0;
      return;
   }
    
   if (&type == &h_ContentDescription)
   {
      delete mDescription;
      mDescription = 0;
      return;
   }

   assert(false);
}

H_ContentType::Type&
Contents::header(const H_ContentType& headerType) const
{
   return mType;
}

H_ContentDisposition::Type&
Contents::header(const H_ContentDisposition& headerType) const
{
   checkParsed();
   if (mDisposition == 0)
   {
      mDisposition = new H_ContentDisposition::Type;
   }
   return *mDisposition;
}

H_ContentTransferEncoding::Type&
Contents::header(const H_ContentTransferEncoding& headerType) const
{
   checkParsed();
   if (mTransferEncoding == 0)
   {
      mTransferEncoding = new H_ContentTransferEncoding::Type;
   }
   return *mTransferEncoding;
}

H_ContentLanguages::Type&
Contents::header(const H_ContentLanguages& headerType) const 
{
   checkParsed();
   if (mLanguages == 0)
   {
      mLanguages = new H_ContentLanguages::Type;
   }
   return *mLanguages;
}

H_ContentDescription::Type&
Contents::header(const H_ContentDescription& headerType) const
{
   checkParsed();
   if (mDescription == 0)
   {
      mDescription = new H_ContentDescription::Type;
   }
   return *mDescription;
}

H_ContentID::Type&
Contents::header(const H_ContentID& headerType) const
{
   checkParsed();
   if (mId == 0)
   {
      mId = new H_ContentID::Type;
   }
   return *mId;
}

// !dlb! headers except Content-Disposition may contain (comments)
void
Contents::preParseHeaders(ParseBuffer& pb)
{
   const char* start = pb.position();
   Data all( start, pb.end()-start);

   Data headerName;

   try
   {
      
   while (!pb.eof())
   {
      const char* anchor = pb.skipWhitespace();
      pb.skipToOneOf(Symbols::COLON, ParseBuffer::Whitespace);
      pb.data(headerName, anchor);

      pb.skipWhitespace();
      pb.skipChar(Symbols::COLON[0]);
      anchor = pb.skipWhitespace();
      pb.skipToTermCRLF();

      Headers::Type type = Headers::getType(headerName.data(), headerName.size());
      ParseBuffer subPb(anchor, pb.position() - anchor);

      switch (type)
      {
         case Headers::ContentType :
         {
            // already set
            break;
         }
         case Headers::ContentDisposition :
         {
            mDisposition = new H_ContentDisposition::Type;
            mDisposition->parse(subPb);
            break;
         }
         case Headers::ContentTransferEncoding :
         {
            mTransferEncoding = new H_ContentTransferEncoding::Type;
            mTransferEncoding->parse(subPb);
            break;
         }
         // !dlb! not sure this ever happens?
         case Headers::ContentLanguage :
         {
            if (mLanguages == 0)
            {
               mLanguages = new H_ContentLanguages::Type;
            }

            subPb.skipWhitespace();
            while (!subPb.eof() && *subPb.position() != Symbols::COMMA[0])
            {
               H_ContentLanguages::Type::value_type tmp;
               header(h_ContentLanguages).push_back(tmp);
               header(h_ContentLanguages).back().parse(subPb);
               subPb.skipLWS();
            }
         }
         default :
         {
            if (isEqualNoCase(headerName, "Content-Transfer-Encoding"))
            {
               mTransferEncoding = new StringCategory();
               mTransferEncoding->parse(subPb);
            }
            else if (isEqualNoCase(headerName, "Content-Description"))
            {
               mDescription = new StringCategory();
               mDescription->parse(subPb);
            }
            else if (isEqualNoCase(headerName, "Content-Id"))
            {
               mId = new Token();
               mId->parse(subPb);
            }
            // Some people put this in ...
            else if (isEqualNoCase(headerName, "Content-Length"))
            {
               mLength = new StringCategory();
               mLength->parse(subPb);
            }
            else if (isEqualNoCase(headerName, "MIME-Version"))
            {
               subPb.skipWhitespace();
               if (!subPb.eof() && *subPb.position() == Symbols::LPAREN[0])
               {
                  subPb.skipToEndQuote(Symbols::RPAREN[0]);
                  subPb.skipChar(Symbols::RPAREN[0]);
               }
               mVersion = subPb.integer();

               if (!subPb.eof() && *subPb.position() == Symbols::LPAREN[0])
               {
                  subPb.skipToEndQuote(Symbols::RPAREN[0]);
                  subPb.skipChar(Symbols::RPAREN[0]);
               }
               subPb.skipChar(Symbols::PERIOD[0]);
               
               if (!subPb.eof() && *subPb.position() == Symbols::LPAREN[0])
               {
                  subPb.skipToEndQuote(Symbols::RPAREN[0]);
                  subPb.skipChar(Symbols::RPAREN[0]);
               }
               
               mMinorVersion = subPb.integer();
            }
            else
            {
               // add to application headers someday
               std::cerr << "Unknown MIME Content- header: " << headerName << std::endl;
               ErrLog(<< "Unknown MIME Content- header: " << headerName);
               assert(false);
            }
         }
      }
   }
   }
   catch (ParseBuffer::Exception & /* e */)
   {
      ErrLog( << "Some problem parsing contents" );
#if 0 // TODO CJ REMOVE this if 0 and make this throw
      throw;
#endif
   }
}

std::ostream&
Contents::encodeHeaders(std::ostream& str) const
{
   if (mVersion != 1 || mMinorVersion != 0)
   {
      str << "MIME-Version" << Symbols::COLON[0] << Symbols::SPACE[0]
          << mVersion << Symbols::PERIOD[0] << mMinorVersion 
          << Symbols::CRLF;
   }

   str << "Content-Type" << Symbols::COLON[0] << Symbols::SPACE[0]
       << mType 
       << Symbols::CRLF;

   if (exists(h_ContentDisposition))
   {
      str <<  "Content-Disposition" << Symbols::COLON[0] << Symbols::SPACE[0];

      header(h_ContentDisposition).encode(str);
      str << Symbols::CRLF;
   }

   if (exists(h_ContentLanguages))
   {
      str <<  "Content-Languages" << Symbols::COLON[0] << Symbols::SPACE[0];

      int count = 0;
      int size = header(h_ContentLanguages).size();

      for (H_ContentLanguages::Type::iterator 
              i = header(h_ContentLanguages).begin();
           i != header(h_ContentLanguages).end(); ++i)
      {
         i->encode(str);

         if (++count < size)
             str << Symbols::COMMA << Symbols::SPACE;
      }
      str << Symbols::CRLF;
   }

   if (mTransferEncoding)
   {
      str << "Content-Transfer-Encoding" << Symbols::COLON[0] << Symbols::SPACE[0]
          << *mTransferEncoding
          << Symbols::CRLF;
   }

   if (mId)
   {
      str << "Content-Id" << Symbols::COLON[0] << Symbols::SPACE[0]
          << *mId
          << Symbols::CRLF;
   }

   if (mDescription)
   {
      str << "Content-Description" << Symbols::COLON[0] << Symbols::SPACE[0]
          << *mDescription
          << Symbols::CRLF;
   }

   if (mLength)
   {
      str << "Content-Length" << Symbols::COLON[0] << Symbols::SPACE[0]
          <<  *mLength 
          << Symbols::CRLF;
   }
   
   str << Symbols::CRLF;
   return str;
}

Data
Contents::getBodyData() const 
{
   ErrLog( << "Need to implement getBodyData function for " << getType() );
   assert(0);
   return Data::Empty;
}


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
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
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
