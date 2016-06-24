#ifndef _QUEUE_H_
#define _QUEUE_H_

// Zeit, bis der naechste dran ist (Default: 15 Sekunden)
#define Q_CYCLE_TIME "qCycleTime"

// Zielraum der Schlange (Default: "/gilden/abenteurer")
#define Q_DEST	     "qDest"

// Laenge der Schlange (Default: 3)
#define Q_LENGTH     "qLength"

// Falls ungleich Null, wird gewartet, bis der zuletzt bewegte Spieler
// den Zielraum verlassen hat, bevor es in der Schlange weitergeht
#define Q_SUSPEND    "qSuspend"

// Meldungen, die beim Aufruecken ausgegeben werden. Vierelementiges Array:
// 1. Meldung, wenn ein Spieler an der Reihe ist
// 2. Meldung fuer den Spieler, der an der Reihe ist
// 3. Meldung, wenn ein Schlangesteher an der Reihe ist
// 4. Meldung fuer Leute ausserhalb der Schlange
// In die erste Meldung kann mit @@wer@@, @@wessen@@, @@wem@@ und @@wen@@ der
// Name des Spielers eingearbeitet werden.
// (Default: ({ "@@wer@@ ist an der Reihe!", "Du bist jetzt an der Reihe!",
//    "Der Naechste ist an der Reihe, und die anderen ruecken auf.",
//    "Die Schlange rueckt weiter. Sogleich stellt sich jemand neu an." }) )
#define Q_MSGS	     "qMsgs"

#define QMSG_OTHER   0
#define QMSG_ME      1
#define QMSG_DEFAULT 2
#define QMSG_OUTSIDE 3

#endif /* _QUEUE_H_ */
