// MorgenGrauen MUDlib
//
// /secure/telnetneg.h -- Definitionen und Prototypes fuer Telnet-Negotiations
//
// $Id: telnetneg.h,v 1.1 2003/02/17 11:20:02 Rikus Exp $

#ifndef _SECURE_TELNETNEG_H_
#define _SECURE_TELNETNEG_H_
// Dies ist vermutlich der einzige Teil, den andere (z.B. player/base.h)
// brauchen...
// Properties fuer die tatsaechliche Fenstergroesse des Telnetclients sowie
// den eingestellten Terminaltyp. Falls der Client die Daten nicht
// herausrueckt, bleiben die Properties leer.
#define P_TTY                "tty"
#define P_TTY_COLS           "tty_cols"
#define P_TTY_ROWS           "tty_rows"
#define P_TTY_TYPE           "tty_type"
#define P_TTY_SHOW           "tty_show"
#define P_TELNET_RTTIME      "p_lib_telnet_rttime"

#endif


#ifdef NEED_PROTOTYPES
#ifndef _SECURE_TELNETNEG_H_PROTOS_
#define _SECURE_TELNETNEG_H_PROTOS_
// wird eigentlich nur fuer die Implementation gebraucht
#include "/sys/telnet.h"

// Konstanten fuer die jew. remote und local handler
#define LOCALON   WILL
#define LOCALOFF  WONT
#define REMOTEON  DO
#define REMOTEOFF DONT

protected int bind_telneg_handler(int option, closure re, closure lo,
                                  int initneg);
protected int do_telnet_neg(int option, int action);
protected varargs int send_telnet_neg(int *arr, int bm_flags);
protected varargs int send_telnet_neg_str(string str, int bm_flags);

#endif // _SECURE_TELNETNEG_H_PROTOS_
#endif // NEED_PROTOTYPES

