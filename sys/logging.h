// MorgenGrauen MUDlib
//
// logging.h
//
// $Id: logging.h 7116 2009-02-06 21:49:42Z Zesstra $

#ifndef __LOGGING_H__
#define __LOGGING_H__

#define SYSLOGPATH    "syslog/"
#define SYSLOG(x)     (SYSLOGPATH+x)

#define SHELLLOGPATH  SYSLOG("shell/")
#define SHELLLOG(x)   (SHELLLOGPATH+x)

#define DAEMONLOGPATH SYSLOG("daemon/")
#define DAEMONLOG(x)  (DAEMONLOGPATH+x)

#define INTERMUDLOGPATH SYSLOG("intermud/")
#define INTERMUDLOG(x) (INTERMUDLOGPATH+x)

#define GUILDLOGPATH  "gilden/"
#define GUILDLOG(x)   (GUILDLOGPATH+x)

#define REPFILEPATH   "report/"
#define REPFILE(x)    (REPFILEPATH+x)

#define ERRORLOGPATH  "error/"
#define ERRORLOG(x)   (ERRORLOGPATH+x)

#endif
