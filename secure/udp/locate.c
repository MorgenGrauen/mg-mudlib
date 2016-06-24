// MorgenGrauen MUDlib
//
// locate.c
//
// $Id: locate.c 6081 2006-10-23 14:12:34Z Zesstra $

#include <udp.h>

#define FOUND		"fnd"
#define USER		"user"
#define VERBOSE		"vbs"

void udp_locate(mapping data) {
    mapping ret;
    object ob;

    ret = ([
	REQUEST: REPLY,
	RECIPIENT: data[SENDER],
	ID: data[ID],
	USER: data[USER],
	VERBOSE: data[VERBOSE],
    ]);
    if (data[DATA] && (ob = find_player(data[DATA])) &&
    interactive(ob) && !ob->query_invis()) {
	ret[FOUND] = 1;
	ret[DATA] = "locate@" + LOCAL_NAME + ": " + ob->short();
    }
    else
	ret[DATA] = "locate@" + LOCAL_NAME + ": No such player: " +
	data[DATA] + "\n";
    INETD->_send_udp(data[NAME], ret);
}
