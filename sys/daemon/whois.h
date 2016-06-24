#ifndef __DAEMON_WHOIS_H__
#define __DAEMON_WHOIS_H__

#define WHOIS         "/p/daemon/udp/whois"

#define DESCR   0
#define COUNTRY 1
#define DATE    2
#define NETNAME 3


// prototypes
#ifdef NEED_PROTOTYPES

mixed whois(string query, int type);
string country(string query);
string city(string query);
string locate(string query, string ip);

#endif /* NEED_PROTOTYPES */
#endif /* __DAEMON_WHOIS_H__ */
