// MorgenGrauen MUDlib
//
// finger.c
//
// $Id: finger.c 6081 2006-10-23 14:12:34Z Zesstra $

#include <udp.h>

string last_finger;

#ifdef ZEBEDEE
#include <system.h>
#elif !defined(INETD)
#define INETD	"/secure/inetd"
#endif

void udp_finger(mapping data)
{
  last_finger=capitalize(to_string(data[SENDER]))+"@"+data[NAME];
  INETD->_send_udp(data[NAME], ([
	REQUEST: REPLY,
	RECIPIENT: data[SENDER],
	ID: data[ID],
	DATA: "/p/daemon/finger"->finger_single(data[DATA])
    ]) );
}

string QueryLastFinger()
{
  return last_finger;
}
