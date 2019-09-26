// MorgenGrauen MUDlib
//
// magier.h -- Magiererweiterung der Shell
//
// $Id: magier.h 9553 2016-05-03 20:22:47Z Zesstra $

#ifndef _MAGIER_H_
#define _MAGIER_H_

#define SHELLDEBUGGER "zesstra"

#define MAX_ARRAY_SIZE   3000 // MUSS EIGENTLICH NACH /sys/config.h oder so

#define ERROR(msg, arg, res)   (printf(msg, arg),res)

#define NO_READ         "%s: Keine Leserechte!\n"
#define NO_WRITE        "%s: Keine Schreibrechte!\n"
#define DOESNT_EXIST    "%s: Datei existiert nicht!\n"
#define ALREADY_EXISTS  "%s: Datei existiert schon.\n"
#define NO_DIRS          "%s ist ein Verzeichnis.\n"
#define SAME_FILE        "%s: Gleiches Quell- und Zielfile: uebergangen.\n"
#define NO_CREATE_DIR   "%s: Verzeichnis konnte nicht erstellt werden.\n"
#define DEST_IS_DIR     "%s: Zieldatei ist ein Verzeichnis.\n"
#define DEST_NO_DELETE  "%s: Zieldatei konnte nicht geloescht werden.\n"
#define NO_MOVE         "%s: Datei konnte nicht verschoben werden.\n"
#define NO_COPY         "%s: Datei konnte nicht kopiert werden.\n"
#define FILE_COPIED     "%s: kopiert.\n"
#define DIR_CREATED     "%s: Verzeichnis erzeugt.\n"
#define FILE_MOVED      "%s: verschoben.\n"
#define FILE_DELETED    "%s wurde geloescht.\n"
#define NO_DELETE       "%s konnte nicht geloescht werden.\n"

#define MAY_READ(x)   call_other(__MASTER_OBJECT__,"valid_read",x,getuid(this_object()))
#define MAY_WRITE(x)  call_other(__MASTER_OBJECT__,"valid_write",x,getuid(this_object()))
#define MAY_DELETE(x) call_other(__MASTER_OBJECT__,"remove_file",x,getuid(this_object()))

#define MAXLEN  50000

#define P_VARIABLES "variables"

#define MAN_OPTS   "mri"
#define MAN_M            1           // Matchen des Begriffs
#define MAN_R            2           // Regexp-Matchen
#define MAN_I            4           // Interaktive Hilfe (deaktiviert)

#define MKDIR_OPTS "vrp"
#define MKDIR_V          1           // Verbose: Ausgeben, was gemacht wird
#define MKDIR_R          2           // Rekursiv alle hoeheren
                                     // Verzeichnisse erstellen
#define MKDIR_P          2           // das gleiche wie MKDIR_R

#define RMDIR_OPTS "v"
#define RMDIR_V          1           // Verbose: Ausgeben, was gemacht wird

#define RM_OPTS   "vrmi"
#define RM_V            1            // Verbose: Ausgeben, was gemacht wird
#define RM_R            2            // Verzeichnisse rekursiv loeschen
#define RM_M            4            // DateiMASKE beachten
#define RM_I            8            // Interaktiv loeschen (immer fragen)

#define UPD_OPTS  "afilrmbvcdhsC"
#define UPD_A            1           // Alle Instanzen bearbeiten
#define UPD_F            2           // Alle Instanzen finden und anzeigen
#define UPD_I            4           // Geerbte Klassen aktualisieren
#define UPD_L            8           // zerstoeren und neu laden
#define UPD_R           16           // neu laden, falls zerstoert
#define UPD_M           32           // alle geerbten Klassen auf
                                     // aktualitaet Testen und
                                     // ggf. aktualisieren
#define UPD_B           64           // Backtrace bei Fehlern
#define UPD_V          128           // Erbschaftslisten mit ausgeben
#define UPD_C          256           // Properties erhalten
#define UPD_D          512           // Objektinhalt komplett destructen
#define UPD_H         1024           // Hard-Destruct (kein remove())
#define UPD_S         2048           // Silent
#define UPD_CONF      4096           // Configure() erzwingen (d.h. kein Update,
                                     // wenn Configure() nicht definiert ist)
#define UPD_LOAD      8192           // laden, falls nicht geladen (nur
                                     // intern fuer 'load)

#define CP_OPTS         "vrfmi"
#define CP_V            1            // Verbose: Ausgeben, was gemacht wird
#define CP_R            2            // Rekursiv kopieren
#define CP_F            4            // Force (nie fragen)
#define CP_M            8            // DateiMASKE beachten
#define CP_I           16            // Interaktiv kopieren (immer fragen)

#define MV_OPTS         "vrfmi"
#define MV_V            1            // Verbose: Ausgeben, was gemacht wird
#define MV_R            2            // Immer auch Dateien im Dir moven
#define MV_F            4            // Force (nie fragen)
#define MV_M            8            // DateiMASKE beachten
#define MV_I           16            // Interaktiv bewegen (immer fragen)

#define CD_OPTS         "ls"
#define CD_L            1            // .readme ausgeben
#define CD_S            2            // .readme unterdruecken
#define CD_B            4            // Zurueck zum letzten Verzeichnis

#define LS_OPTS         "alrtsug"
#define LS_A            1            // ALLE Files anzeigen
#define LS_L            2            // Lange Ausgabe
#define LS_R            4            // Rueckwaerts ausgeben
#define LS_T            8            // Nach Zeit ordnen
#define LS_S            16           // Nach Groesse ordnen
#define LS_U            32           // UID anzeigen
#define LS_G            64           // Gruppe anzeigen

#define CLONE_OPTS      "f"
#define CLONE_F         1            // Auf jeden Fall versuchen zu clonen

#define GREP_OPTS       "chilnvrmf"
#define GREP_C          1            // Nur Zahl der Zeilen ausgeben
#define GREP_H          2            // Dateinamen nicht mit ausgeben
#define GREP_I          4            // Gross-/Kleinschreibung ignorieren
#define GREP_L          8            // Dateinamen immer mit ausgeben
#define GREP_N          16           // Zeilennummern mit ausgeben
#define GREP_V          32           // Nur Zeilen matchen, die NICHT passen
#define GREP_R          64           // Rekursiv suchen
#define GREP_M          128          // Dateimaske angegeben
#define GREP_F          256          // Ausgabe in Datei

#define ACCESS_OPTS     "arlfgm"
#define ACCESS_A        1            // Berechtigung hinzufuegen
#define ACCESS_R        2            // Berechtigung entfernen
#define ACCESS_L        4            // Berechtigungen anzeigen
#define ACCESS_F        8            // Berechtigung fuer Dateien anzeigen
#define ACCESS_G        16           // Das ganze fuer Gruppen
#define ACCESS_M        32           // Das ganze fuer Gruppenleiter

// Funktioniert nur, wenn SNOOP_L == SF_LOCKED und SNOOP_F == SF_FORCE
// (Definitionen aus /sys/snooping.h)
#define SNOOP_OPTS      "lf"
#define SNOOP_L         1            // Niedriger Magier kann sich nicht
                                     // dazwischenklinken
#define SNOOP_F         2            // Snoope snoopenden Magier, wenn der
                                     // Charakter schon gesnoopt wird

#define NO_CHECK        M_GO|M_SILENT|M_NO_SHOW|M_NO_ATTACK|M_NOCHECK

#define INV_SAVE        "/room/void.c"

#define USAGE(str)      (_notify_fail(break_string(sprintf("Syntax: %s\n", str),77)), 0)

#define BASENAME 0
#define FILESIZE 1
#define FILEDATE 2
#define FULLNAME 3
#define PATHNAME 4
#define DESTNAME 5
#define SUBDIRSIZE 6

#define RET_OK 1
#define RET_FAIL 0
#define RET_JUMP -1
#define RET_DELETE -2

#define MODE_LSA   0
#define MODE_LSB   1
#define MODE_CP    2
#define MODE_MV    3
#define MODE_RM    4
#define MODE_CD    5
#define MODE_CAT   6
#define MODE_GREP  7
#define MODE_ED    8
#define MODE_UPD   9
#define MODE_MORE  10
#define MODE_RMDIR 11
#define MODE_CLONE 12

#define COLORS ([ "none": ANSI_NORMAL, \
                  "bold": ANSI_BOLD, \
                  "blink": ANSI_BLINK, \
                  "invers": ANSI_INVERS, \
                  "black": ANSI_BLACK, \
                  "red": ANSI_RED, \
                  "green": ANSI_GREEN, \
                  "yellow": ANSI_YELLOW, \
                  "blue": ANSI_BLUE, \
                  "purple": ANSI_PURPLE, \
                  "cyan": ANSI_CYAN, \
                  "white": ANSI_WHITE, \
                  "bblack": ANSI_BG_BLACK, \
                  "bred": ANSI_BG_RED, \
                  "bgreen": ANSI_BG_GREEN, \
                  "byellow": ANSI_BG_YELLOW, \
                  "bblue": ANSI_BG_BLUE, \
                  "bpurple": ANSI_BG_PURPLE, \
                  "bcyan": ANSI_BG_CYAN, \
                  "bwhite": ANSI_BG_WHITE, \
                ])

#define VALID_READ_CL  symbol_function("valid_read",__MASTER_OBJECT__)
#define VALID_WRITE_CL symbol_function("valid_write",__MASTER_OBJECT__)
#define CREATOR_CL  symbol_function("creator_file",__MASTER_OBJECT__)

#define DIR "directory"
#define OBJ "loaded"
#define VC  "virtual"

#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG(x) if (find_player(SHELLDEBUGGER)) \
                     tell_object(find_player(SHELLDEBUGGER),x+"\n")

#endif // _MAGIER_H_


#ifdef NEED_PROTOTYPES
#ifndef _MAGIER_H_PROTOS_
#define _MAGIER_H_PROTOS_
static string *parseargs(string cmdline,int flags,string opts,int build_fn);
static varargs void asynchron(mixed* array, closure cmd, mixed data, mixed flags, int c);
static varargs mixed *file_list(string *files, int mode, int recursive, string dest, string mask);
static mixed to_filename(string str);
static int _verfolge(string str);
static string glob2regexp(string str);

varargs public void More(string txt, int file,mixed ctrl, mixed *ctrlargs, int flags);
#endif // _MAGIER_H_PROTOS_
#endif // NEED_PROTOTYPES


