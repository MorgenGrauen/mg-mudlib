// MorgenGrauen MUDlib
/** \file /std/player/pklog.c
* Funktion zur Detektion und Loggen von Angriffen von Spielern auf Spieler.
* \author Zesstra
* \date 12.08.2008
* \version $Id$
*/
/* Changelog:
*/
#pragma strong_types
#pragma save_types
#pragma no_clone
#pragma no_shadow
#pragma pedantic
#pragma range_check

#include <defines.h>
#include <commands.h>
#include <wizlevels.h>
#include <player/base.h>

#define RNAME(x) capitalize(getuid(x))

/** Ist victim in einer Arena oder Schattenwelt?
  */
nomask int CheckArenaFight(object victim) {
  return (object_name(environment(victim))[0..14]=="/d/schattenwelt");
}

nomask protected int CheckPlayerAttack(object attacker, object victim,
                                       string angriffsmsg)
{
  // falls mal jemand ne Closure auf diese Funktion in die Finger bekommt und
  // protected umgeht.
  if (extern_call())
      raise_error(sprintf("Extern Call to CheckPlayerAttack in %O!\n",ME));

  // nur Spieler sind interessant.
  if ( query_once_interactive(attacker) && query_once_interactive(victim)
      && !( IS_LEARNER(attacker) && IS_LEARNER(victim) ) ) {
    string filemessage, wizshout;
    int arena;

    // Arena- oder Schattenweltkampf?
    arena=CheckArenaFight(attacker);

    wizshout = sprintf("\n**** %s greift %s an. (%s) ***\n",
        RNAME(attacker), RNAME(victim), object_name(ME));
    filemessage = sprintf("\n[%s] %s greift %s an. (%s) %s %s\n",
        strftime("%d%m%y-%H:%M:%S",time()), RNAME(attacker),
        RNAME(victim), object_name(this_object()),
        (arena ? "(ARENA)" : ""),
        (victim->QueryProp(P_TESTPLAYER) ? "(Testspieler)" : ""));

    // Angriffsmsg vom Aufrufer anhaengen.
    if (stringp(angriffsmsg) && sizeof(angriffsmsg)) {
      wizshout += angriffsmsg;
      filemessage += angriffsmsg;
    }
    // ggf. echten TI anhaengen oder warnen, falls keiner existiert.
    if ( this_interactive() != attacker ) {
        if ( this_interactive() ) {
          wizshout += "ACHTUNG: TI = " + getuid(this_interactive())
            +"\n";
          filemessage += "ACHTUNG: TI = "+getuid(this_interactive())
            +"\n";
        }
        else {
            filemessage += " ACHTUNG: Kein TI vorhanden!\n";
            wizshout += " ACHTUNG: Kein TI vorhanden!\n";
        }
    }
    // caller_stack() mitloggen (aber nicht Magier vollscrollen).
    filemessage += "Callerstack: " + CountUp(map(caller_stack(1),
          function string (object po) {return to_string(po);}),
        ", ", ", ") + "\n";
    // Commandstack anhaengen
    mixed cstack = command_stack();
    filemessage += "Commandstack: " + CountUp(map(cstack,
          function string (mixed arr) {
              return sprintf("({Original-TP: %O, TP: %O, Kommando: %s})",
                arr[CMD_ORIGIN],arr[CMD_PLAYER],arr[CMD_TEXT] || "");
          },", ",", ")) + "\n";
    // fuer Magier originaeren Befehl anhaengen:
    if (sizeof(cstack))
      wizshout += sprintf("Kommando: %s\n",
          cstack[0][CMD_TEXT] || "<unbekannt>");

    wizshout += "\n";

    // erstmal loggen
    if ( arena )
        log_file("ATTACKS_ARENA", filemessage);
    else 
        log_file("ATTACKS", filemessage);

    // dann Magiern bescheidgeben
    if ( !arena && !(victim->QueryProp(P_TESTPLAYER)) ) {
      foreach(object wiz: users()) {
        if ( IS_LORD(wiz) || IS_DEPUTY(wiz) )
          tell_object(wiz, wizshout);
      }
    }
    return 1; // Spieler-Spielerkampf
  }
  return 0; // kein Spieler-Spielerkampf
}

