// Konfigurationsdatei fuer die Sprachausgabe des INETD

#ifndef __UDP_LANG_H__
#define __UDP_LANG_H__

// ***
// *** inetd.c
// ***

#define INETD_INV_ACCESS_MSG "%s: Illegaler Zugriff auf die x-Funktion.\n"
//                            time                         function

// ***
// *** channel.c
// ***

#define INETD_INV_CHAN_MSG "\n%s: %s@%s versucht, auf Kanal \'%s\' zu senden.\n"
//                          time who mud                  channel
#define INETD_NO_SUCH_CHAN "[Root@%s] Es gibt hier keinen Intermud-Kanal %s.\n"
//                                mud                                  channel
#define INETD_CHAN_LISTEN  "[%s@%s] Auf dem Kanal befinde%s sich: %s\n"
//                           channel mud                plural    names
#define INETD_CHAN_NO_LIST \
                         "[%s@%s] Es befindet sich niemand auf diesem Kanal.\n"
//                         channel mud
#define INETD_CHAN_NO_HIST "[%s@%s] Es ist keine Geschichte verfuegbar.\n"
//                          channel mud
#define INETD_CHAN_HIST    "[%s@%s] History:\n"
//                          channel mud

// ***
// *** locate.c
// ***

// ***
// *** man.c
// ***

#define INETD_MANPAGE_FOUND "[ManD@%s] Folgende Seite wurde gefunden: %s\n"
//                              mud                                 file
#define INETD_NO_MANPAGE    "[ManD@%s] Keine Hilfeseite gefunden fuer '%s'.\n"
//                              mud                                 page
#define INETD_MANPAGES      "[ManD@%s] Die folgenden Seiten passen:\n" \
                             "%'-'78.78s\n%s%'-'78.78s\n"
//                                mud  "" pagelist ""
#define INETD_MAN_REQUESTED "%s@%s: Abfrage abgeschickt.\n"
//                             page  mud

// ***
// *** newhost.c
// ***


// ***
// *** query.c
#define INETD_QUERY_REQUESTED "%s@%s: Abfrage abgeschickt.\n"
// ***


// ***
// *** reply.c
// ***

#endif // __UDP_LANG_H__
