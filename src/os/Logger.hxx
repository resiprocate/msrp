#if !defined(MSRP_LOGGER_HXX)
#define MSRP_LOGGER_HXX 

#include <iosfwd>

#include "src/os/Log.hxx"
#include "src/os/Lock.hxx"

#ifdef WIN32

#include "src/os/DataStream.hxx"

#include "src/os/Data.hxx"

#include <windows.h>

#endif 



/**
   Defines a set of logging macros, one for each level of logging.

   Example:
#include Logger.hxx
#define RESIPROCATE_SUBSYSTEM msrp::Subsystem::SIP
   ...
   DebugLog(<< "hi there " << mix << 4 << types);  // note leading << and no endl
*/


#define DELIM 



// unconditionally output to cerr -- easily change back and forth
#define CerrLog(args_)                                                  \
  msrp::Log::tags(msrp::Log::StdErr, RESIPROCATE_SUBSYSTEM,           \
                   __FILE__, __LINE__, std::cerr) << ' ' << '|' << ' '  \
          args_ << std::endl;

#define StackLog(args_)                                                         \
GenericLog(RESIPROCATE_SUBSYSTEM, msrp::Log::Stack, args_)

#define DebugLog(args_) \
GenericLog(RESIPROCATE_SUBSYSTEM, msrp::Log::Debug, args_)

#define InfoLog(args_) \
GenericLog(RESIPROCATE_SUBSYSTEM, msrp::Log::Info, args_)

#define WarningLog(args_) \
GenericLog(RESIPROCATE_SUBSYSTEM, msrp::Log::Warning, args_)

#define ErrLog(args_) \
GenericLog(RESIPROCATE_SUBSYSTEM, msrp::Log::Err, args_)

#define CritLog(args_) \
GenericLog(RESIPROCATE_SUBSYSTEM, msrp::Log::Crit, args_)

// do/while allows a {} block in an expression
#define GenericLog(system_, level_, args_)                                                      \
do                                                                                              \
{                                                                                               \
   const msrp::Log::ThreadSetting* setting = msrp::Log::getThreadSetting();                   \
   if ((setting && level_ <= setting->level) ||                                                 \
       (!setting && msrp::GenericLogImpl::isLogging(level_)))                                  \
   {                                                                                            \
      char _resip_buffer[8096];                                                                 \
      msrp::Data _resip_result(msrp::Data::Share, _resip_buffer, sizeof(_resip_buffer));      \
      _resip_result.clear();                                                                    \
      {                                                                                         \
         msrp::DataStream _resip_strm(_resip_result);                                          \
         if (!msrp::Log::getExternal())                                                        \
         {                                                                                      \
            msrp::Log::tags(level_, system_, __FILE__, __LINE__, _resip_strm)                  \
               << " | ";                                                                        \
         }                                                                                      \
         _resip_strm args_;                                                                     \
      }                                                                                         \
      /* checking external twice is better than repeating macro args */                         \
      if (msrp::Log::getExternal())                                                            \
      {                                                                                         \
         (*msrp::Log::getExternal())(level_, system_, msrp::Log::getAppName(),                \
                                      __FILE__, __LINE__, _resip_result);                       \
          break;                                                                                \
      }                                                                                         \
                                                                                                \
      msrp::Lock lock(msrp::Log::_mutex);                                                     \
      if (msrp::Log::_type == msrp::Log::VSDebugWindow)                                       \
      {                                                                                         \
         _resip_result += "\r\n";                                                               \
         msrp::GenericLogImpl::OutputToWin32DebugWindow(_resip_result);                        \
      }                                                                                         \
      else                                                                                      \
      {                                                                                         \
         /* endl is magic in syslog -- so put it here */                                        \
         msrp::GenericLogImpl::Instance() << _resip_result << std::endl;                       \
      }                                                                                         \
   }                                                                                            \
} while (0)

#ifdef NO_DEBUG
// Suppress debug logging at compile time
#undef DebugLog
#define DebugLog(args_)
#undef StackLog(args_)
#define StackLog(args_)
#endif

namespace msrp
{

class GenericLogImpl :  public Log 
{
   public:
      static std::ostream& Instance();
      static bool isLogging(Log::Level level) ;
      static unsigned int MaxLineCount;
      static void OutputToWin32DebugWindow(const Data& result);

   private:
      static std::ostream* mLogger;
      static unsigned int mLineCount;
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
