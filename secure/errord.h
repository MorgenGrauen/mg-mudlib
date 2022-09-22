/*  MorgenGrauen MUDlib
    /sys/errord.h
    Headerfile fuer den Error-Daemonen
    Autor: Zesstra
    $Id: errord.h 9439 2016-01-20 09:48:28Z Zesstra $
    ggf. Changelog:
*/

#ifndef __ERRORD_H__
#define __ERRORD_H__

// Typen
#define T_RTERROR   1	   //Runtime Error / Laufzeitfehler
#define T_RTWARN    2    //Runtime Warning / Laufzeitwarnung
#define T_CTERROR   4    //Compiletime Error / Fehler beim Uebersetzen
#define T_CTWARN    8    //Compiletime Warning / Warnung beim Uebersetzen
#define T_REPORTED_ERR   16   // von Spielern gemeldete Bugs
#define T_REPORTED_TYPO  32   // von Spielern gemeldete Typos
#define T_REPORTED_IDEA  64   // von Spielern gemeldete Ideen 
#define T_REPORTED_MD    128  // von Spielern gemeldete fehlende Details
#define T_REPORTED_SYNTAX 256 // Von Spielern gemeldete Syntaxprobleme
#define ALL_ERR_TYPES ({T_RTERROR, T_RTWARN, T_CTERROR, T_CTWARN, \
                        T_REPORTED_ERR, T_REPORTED_TYPO, T_REPORTED_IDEA, \
                        T_REPORTED_MD, T_REPORTED_SYNTAX })
#define CHANGEABLE_TYPES ({T_REPORTED_ERR, T_REPORTED_TYPO, T_REPORTED_IDEA, \
                           T_REPORTED_MD, T_REPORTED_SYNTAX})

// Status
#define STAT_DELETED  0x1
#define STAT_LOCKED   0x2
#define STAT_RESOLVED 0x4

// Achtung: viele Keys koennen nicht vorhanden oder Werte 0 sein!
#define F_ID          "id"          //int: (row) ID in der DB
#define F_TYPE        "type"        //int: Typ-Werte, s.o.
#define F_HASHKEY     "hashkey"     //string: Hashkey des Fehlers
#define F_UID         "uid"         //string: "UID des Fehlers"
#define F_STATE       "state"       //int: Status-Werte, s.o.
#define F_MODSTAMP    "mtime"       //int
#define F_CREATESTAMP "ctime"       //int
#define F_READSTAMP   "atime"       //int 
#define F_PROG        "prog"        //string == Bluename, falls kein replace_program()
#define F_OBJ         "obj"         //string
#define F_LOADNAME    "loadname"    //string == Bluename, falls kein rename_object()
#define F_LINE        "loc"         //int
#define F_MSG         "message"     //string
#define F_HB_OBJ      "hbobj"       //string
#define F_CAUGHT      "caught"      //int
#define F_TITP        "titp"        //string
#define F_STACK       "stack"       //Array von mixed (mixed)
#define F_CLI         "command"     //string (Spielereingabe)
#define F_VERB        "verb"        //string (Kommandoverb)
#define F_COUNT       "count"       //int
#define F_TIENV       "tienv"       //string, object_name() vom Env von TI/TP
#define F_LOCK        "locked"      //mixed (Array von 2 Elementen)
#define F_RESOLVER    "resolver"    //string (wer hat gefixt?)
#define F_NOTES       "notes"       //Bemerkungen, Array von 3-elementigen Arrays

// Prop, welche in /players/ das Loggen von Fehlern im errord unterbindet.
#define P_DONT_LOG_ERRORS    "p_lib_errord_dont_log"

#ifdef __NEED_IMPLEMENTATION__

#ifdef DEBUG
#undef DEBUG
#endif
//#define DEBUG(x)
#define DEBUG(x)  if (funcall(symbol_function('find_player),"zesstra"))\
        tell_object(funcall(symbol_function('find_player),"zesstra"),\
        "EDBG: "+x+"\n")

//Wer pflegt das Ding hier gerade?
#define MAINTAINER ({"zesstra"})

//Stanard-Expire
#define STDEXPIRE 2678400 //31 Tage

//Blacklist fuer Files, die nicht erfasst werden sollen (nur
//Uebersetzungsprobleme momentan)
#define BLACKLIST ({".tool.lpc",".xtool.h"})

//Zugriffsarten
#define M_READ   1
#define M_WRITE  2  // actually: append-only
#define M_FIX    4
#define M_REASSIGN 8
#define M_DELETE 16
#define M_CHANGE_TYPE 32

// Changelog
#define CHANGELOG "/log/CHANGELOG"

// Log
//#define MAILLOG       "sent.log"

// Standardmailtext: TODO 
//#define STANDARDTEXT  HOME("mailtext.txt")

#define STANDARDMAILTEXT "Huhu lieber Mitmagier,\n\n" \
    "der unten angegebene Fehler in einem Objekt, fuer welches Du " \
    "(als programmierender Magier oder RM) zustaendig bist, wurde soeben " \
    "von %s als gefixt markiert. Bitte beachte ggf. die unten angebenen " \
    "Bemerkungen zum Fix.\n\n--- Fehler-Daten ---\n"

#define STANDARDMAILTEXT_ERRORHINT "Huhu %s,\n\n" \
    "ein von Dir abgesetzter Fehler (s.u.) wurde von %s bearbeitet und als " \
    "erledigt markiert. Bei Fragen wende Dich bitte an den " \
    "bearbeitenden Magier.\n" \
    "Vielen Dank fuer Deine Mithilfe!\n\n" \
    "--- Fehler-Daten ---\n"


#endif // __NEED_IMPLEMENTATION__

#endif // __ERRORD_H__

