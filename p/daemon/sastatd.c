// MorgenGrauen MUDlib
/** \file /p/daemon/sastatd.c
* Statistikdaemon fuer Skill-Attribute
* Der Daemon loggt SA-Modifier und schreibt sie von Zeit zu Zeit auf die
* Platte.
* \author Zesstra
* \date 06.08.2008
* \version $Id$
*/
/* Changelog:
*/
#pragma strong_types,save_types
#pragma no_clone,no_inherit,no_shadow
#pragma pedantic,range_check

#include <defines.h>

/** anfaengliche Groesse des Log-Puffers
  */
#define DEFAULT_BUFFER_LEN 200
/** ab dieser Anzahl an Eintraegen wird im naechsten Backend-Zyklus vom Driver
 * per Callout der Puffer weggeschrieben. */
#define BUFFERTHRESHOLD 150

/** Logfile fuer die Stats
  */
#define STATLOG "SASTATS"
/** max. Logfilegroesse (3 MB)
  */
#define MAXLOGSIZE 3145728

struct log_entry_s {
  int timestamp;
  string liv;
  string caster;
  string atrname;
  mixed value;
  int duration;
};

struct log_buffer_s {
  mixed buf;  // Array von log_entry
  int bufferlen; // Laenge des Buffers, kann dyn. vergroessert werden.
  int sp; // oberster Eintrag im Buffer (letztendlich nen Stack).
};

/** speichert Logeintraege mit indiv. Modifiereintraegen.
 * Array von log_entry_s mit Info ueber Pufferlaenge und zuletzt
 * geschriebenem Arrayeintrag.
*/
struct log_buffer_s logbuf = (<log_buffer_s>
    buf : allocate(DEFAULT_BUFFER_LEN),
    bufferlen : DEFAULT_BUFFER_LEN,
    // -1, weil der sp beim Loggen zuerst erhoeht wird und sonst Elemente 0
    // leer bleibt.
    sp : -1 );

/** \def DEBUG
  Outputs debug message to Maintainer, if Mainteiner is logged in. 
*/
#ifndef DEBUG
#define DEBUG(x)  if (find_player("zesstra"))\
            tell_object(find_player("zesstra"),\
                                      "EDBG: "+x+"\n")
#endif

private string FormatLogEntry(struct log_entry_s entry) {
  return sprintf("[%s] %s, %s, %s, %s, %d\n", 
      strftime("%d%m%y-%H%M%S",entry->timestamp),
      entry->liv, entry->caster, entry->atrname,
      entry->value, entry->duration);
}

private void WriteBuffer() {
  // relevanten Teil des Puffers extrahieren.
  mixed buf = logbuf->buf[0..logbuf->sp];
  // dann alle Eintraege in Strings mappen
  buf = map(buf, #'FormatLogEntry);
  // wegschreiben
  log_file(STATLOG, sprintf("%@s",buf), MAXLOGSIZE);
  // sp auf den Pufferanfang setzen
  logbuf->sp = -1;
}

public void LogModifier(object caster, string atrname,
                        mixed value, int duration) {
  // nur Spieler sind interessant
  if (!query_once_interactive(previous_object()))
    return;

  // sp um eins erhoehen und schauen, ob die Puffergroesse ueberschritten
  // wurde. Wenn ja, Puffer vergroessern.
  logbuf->sp++;
  if (logbuf->sp >= logbuf->bufferlen) {
    logbuf->buf = logbuf->buf + allocate(50);
    logbuf->bufferlen += 50;
  }
  // eintragen. Interessanterweise ist es tatsaechlich kein Problem, wenn
  // Structmember und lokale Variablen hier den gleichen Namen habem. ;-)
  logbuf->buf[logbuf->sp] = (<log_entry_s>
      timestamp : time(),
      liv : getuid(previous_object()), // nur Spieler
      caster : object_name(caster),
      atrname : atrname,
      value : to_string(value), // wegen (fluechtiger) Closures
      duration : duration );
  // wegschreiben veranlassen
  if (logbuf->sp > BUFFERTHRESHOLD
      && find_call_out(#'WriteBuffer) == -1)
    call_out(#'WriteBuffer, 0);
}

protected void create() {
  seteuid(getuid(ME));
}

varargs public int remove(int silent) {
  if (logbuf->sp > -1)
    WriteBuffer();
  destruct(ME);
  return 1;
}

void reset() {
  // ggf. Puffer wegschreiben.
  if (logbuf->sp > BUFFERTHRESHOLD)
    WriteBuffer();
  // ggf. sehr grosse Puffer verkleinern
  if (logbuf->bufferlen > DEFAULT_BUFFER_LEN * 2) {
    logbuf->buf = allocate(DEFAULT_BUFFER_LEN);
    logbuf->bufferlen = DEFAULT_BUFFER_LEN;
    logbuf->sp = -1;
  }
}

