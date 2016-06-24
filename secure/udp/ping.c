// MorgenGrauen MUDlib
//
// ping.c
//
// $Id: ping.c 6081 2006-10-23 14:12:34Z Zesstra $

#include <udp.h>

#ifdef ZEBEDEE
#include <system.h>
#elif !defined(INETD)
#define INETD	"/secure/inetd"
#endif

void udp_ping(mapping data) {
    INETD->_send_udp(data[NAME], ([
	REQUEST: REPLY,
	RECIPIENT: data[SENDER],
	ID: data[ID],
	DATA: LOCAL_NAME + " is alive.\n"
    ]) );
}
