// haus.h -- Definitionen fuer die Seherhaeuser
//
//  Grundobjekt (c) 1994 Boing@MorgenGrauen
//  Abschliessen und Rauswerfen von Jof
//  Fuer Aenderungen ab dem 06.10.94 verantwortlich: Wargon
//
// $Id: haus.h,v 1.2 2003/05/03 13:09:05 mud Exp $
//
#ifndef __HAUS_H__
#define __HAUS_H__

// #define DEBUG(x)	if (find_player("vanion")) tell_object(find_player("vanion"),x);
//#define HDEBUG(x) if(find_player("vanion")) \
//	              tell_object(find_player("vanion"),break_string((x),78,"HDBG: ",1))
#define HDEBUG(x)

// Maintainer wird u.a. benutzt, um die access_rights zu regeln
#define MAINTAINER ({"vanion"})
#define IS_MAINTAINER(x) (member(MAINTAINER, getuid(x))!=-1)

/*** Pfade ***/
#define HAEUSERPFAD     "/d/seher/haeuser/"
#define HAUSSAVEPATH	"/data"+HAEUSERPFAD+"save/"
#define SPECIALPATH	HAEUSERPFAD+"special/"
#define VERWALTER	(HAEUSERPFAD+"hausverwalter")
#define HAUS		HAEUSERPFAD+"haus"
#define RAUM		HAEUSERPFAD+"raum"
#define SAVEFILE	HAUSSAVEPATH+"haeuser"
#define HAUSNAME(x)	(HAEUSERPFAD+x+"haus")
#define RAUMNAME(x,n)	(HAEUSERPFAD+x+"raum"+to_string(n))
#define REPFILE(x)	("/data"+HAEUSERPFAD+"rep/"+x+".rep")
#define HLOG(f,x) write_file("/log/seher/haeuser/"+(f), dtime(time())+" - "+(x))

/* PATH bitte nicht mehr benutzen!
 * Dieser #define existiert nur noch aus Kompatibilitaetsgruenden.
 */
#ifndef PATH
#define PATH		HAEUSERPFAD
#endif

/*** Module ***/
#define USERCMD HAEUSERPFAD+"modules/usercmd"
#define LOSA	HAEUSERPFAD+"modules/losa"
#define HAUSTUER HAEUSERPFAD+"modules/haustuer"

/*** Bausparvertrag und Ratenblock ***/
#define SUMME_L 30*RATENHOEHE	  /* Soviel kostet das gute Stueck (langsam) */
#define SUMME_S 25*RATENHOEHE	  /* Und soviel bei der schnellen Variante */
#define RATENHOEHE 80000  /* Einzahlbar per Ratenblock */
#define MAX_TIME 7200	  /* Laufzeit der Raten: 4 Stunden = 7200 heart_beats */
#define WARN_TIME 450	  /* Abstand zwischen den Warnungen: 15 Minuten */

#define V_MONEY 0   // Das schon eingezahlte Geld
#define V_DLINE 1   // die Deadline
#define V_WTIME 2   // Zeit bis zur naechsten Erinnerung
#define V_FLAGS 3   // Diverse Flags

#define V_SIGNED 1  // Vertrag schon unterschrieben
#define V_FAST	 2  // Schnelle Variante
#define V_COMPL  4  // Vertrag erfuellt
#define V_RAUM	 8  // Ausbauvertrag
#define V_EP	16  // EP-Vertrag

#define B_FAST	 1  // Block gehoert zur schweren Variante
#define B_EXTEND 2  // Strafzeit laeuft
#define B_ACTIVE 4  // Shadow laden
#define B_SHADOW 8  // Shadow laeuft...
#define B_EP	16  // EP-Vertrag

/*** Tuerstatus ***/
#define D_CLOSED 1
#define D_LOCKED 2

/*** Beschreibungsflags ***/
#define AUSSEN	1
#define KURZ	2
#define LANG	4
#define DETAIL	8
#define RDETAIL 16
#define BEFEHL	32  // Nur aendern
#define REPORT	64  // Nur aendern

/*** Flags fuer HausProp() ***/
#define HP_ENV	    0 // Name des Raums, in dem das Haus steht (string)
#define HP_ROOMS    1 // Anzahl der Raeume, die das Haus besitzt (int)
#define HP_ALLOWED  2 // Wer hat Sonderbefugnisse im Haus? (string *)

/*** Vermischtes ***/
#define H_CHEST "h_chest"         // Wenn gesetzt, steht in diesem Raum die Truhe
#define TRUHE "\t\ruhe"           // Und diese ID hat die Truhe in jedem Fall.
#define H_COMMANDS "h_commands"   // Enthaelt mapping mit Befehlen.
#define H_DOOR "h_door"           // Enthaelt die Beschreibung der Tuer.
#define H_DOORSTAT "h_doorstat"   // Enthaelt den Zustand der Haustuer.
#define H_DOORLSTAT "h_doorlstat" // Tuerzustand als Stringarray.
#define H_REPORT "h_report"       // Info ueber typo/bug/idee im Haus
#define H_SPECIAL "h_special"     // Besonderheiten im Haus
#define H_FURNITURE "h_furniture" // Rebootfeste Sehermoebel

/*** Typen fuer H_SPECIAL ***/
#define HS_ITEM 1  // ein bestimmtes Objekt (per AddItem())
#define HS_EXIT 2  // ein Ausgang (per AddExit())

/*** Einstellungen fuer Furniture ***/
#define MAX_FURNITURE_PER_ROOM 5

/*** Last but not least... ***/
#define UP_ARGS(o) ((o)->_unparsed_args(1))

#endif	// __HAUS_H__

#ifdef NEED_PROTOTYPES

#ifndef __HAUS_H_PROTOS__
#define __HAUS_H_PROTOS__

varargs void Save(int crunched);
void Load();
varargs string QueryOwner(int withNr);
varargs string SetOwner(string o, int nr);
protected void AddExitNoCheck(mixed cmd, mixed dest);
void RemoveExitNoCheck(mixed cmd);
int allowed_check(object _test);

#endif	/* __HAUS_H_PROTOS__ */
#endif	/* NEED_PROTOTYPES */

// $Log: haus.h,v $
// Revision 1.2  2003/05/03 13:09:05  mud
// PATH in HAEUSERPFAD umbenannt
//
// Revision 1.1.1.1  2000/08/20 20:22:42  mud
// Ins CVS eingecheckt
//
// Revision 2.7  1995/06/28  08:58:07  Wargon
// H_REPORT fuer Spielerrueckmeldungen.
// Flags BEFEHL und REPORT fuer Aenderungen.
//
// Revision 2.6  1995/04/21  08:53:57  Wargon
// #defines fuer Modularisierung und neue Tuerfeatures.
//
// Revision 2.5  1995/02/27  20:54:49  Wargon
// UP_ARGS angepasst, da Parser jetzt auch fuer Spieler aktiv.
//
// Revision 2.3  1995/02/15  11:15:08  Wargon
// H_DOOR: Enthaelt die Beschreibung der Haustuer.
//
// Revision 2.2  1995/02/04  17:18:39  Wargon
// H_COMMANDS: enthaelt mapping mit selbstdefinierten Hausbefehlen.
// CHEST in H_CHEST umbenannt.
//
// Revision 2.1  1995/02/04  15:06:18  Wargon
// Definition von CHEST als Property fuer den Standort der Truhe und
// TRUHE als feste ID der Truhe.
//
// Revision 2.0  1995/02/01  20:45:18  Wargon
// UP_ARGS-Makro, liefert fuer ALLE das Ergebnis von _unparsed_args(1)
//
