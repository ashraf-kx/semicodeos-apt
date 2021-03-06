// -*- mode: cpp; mode: fold -*-
// Description								/*{{{*/
/* ######################################################################
   
   Proxy - Proxy releated functions
   
   ##################################################################### */
									/*}}}*/
// Include Files							/*{{{*/
#include<apt-pkg/configuration.h>
#include<apt-pkg/error.h>
#include<apt-pkg/fileutl.h>
#include<apt-pkg/strutl.h>

#include<iostream>
#include <unistd.h>

#include "proxy.h"
									/*}}}*/

// AutoDetectProxy - auto detect proxy					/*{{{*/
// ---------------------------------------------------------------------
/* */
bool AutoDetectProxy(URI &URL)
{
   // we support both http/https debug options
   bool Debug = _config->FindB("Debug::Acquire::"+URL.Access,false);

   // the user already explicitly set a proxy for this host
   if(_config->Find("Acquire::"+URL.Access+"::proxy::"+URL.Host, "") != "")
      return true;

   // option is "Acquire::http::Proxy-Auto-Detect" but we allow the old
   // name without the dash ("-")
   std::string AutoDetectProxyCmd = _config->Find("Acquire::"+URL.Access+"::Proxy-Auto-Detect",
                                      _config->Find("Acquire::"+URL.Access+"::ProxyAutoDetect"));

   if (AutoDetectProxyCmd.empty())
      return true;

   if (Debug)
      std::clog << "Using auto proxy detect command: " << AutoDetectProxyCmd << std::endl;

   std::string const urlstring = URL;
   std::vector<const char *> Args;
   Args.push_back(AutoDetectProxyCmd.c_str());
   Args.push_back(urlstring.c_str());
   Args.push_back(nullptr);
   FileFd PipeFd;
   pid_t Child;
   if(Popen(&Args[0], PipeFd, Child, FileFd::ReadOnly, false) == false)
      return _error->Error("ProxyAutoDetect command '%s' failed!", AutoDetectProxyCmd.c_str());
   char buf[512];
   if (PipeFd.ReadLine(buf, sizeof(buf)) == nullptr)
      return true;
   PipeFd.Close();
   ExecWait(Child, "ProxyAutoDetect", true);
   auto const cleanedbuf = _strstrip(buf);

   if (cleanedbuf[0] == '\0')
      return _error->Warning("ProxyAutoDetect returned no data");

   if (Debug)
      std::clog << "auto detect command returned: '" << cleanedbuf << "'" << std::endl;

   if (strstr(cleanedbuf, URL.Access.c_str()) == cleanedbuf)
      _config->Set("Acquire::"+URL.Access+"::proxy::"+URL.Host, cleanedbuf);

   return true;
}
									/*}}}*/
