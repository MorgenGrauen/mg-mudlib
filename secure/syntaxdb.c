// MorgenGrauen MUDlib
/** \file /file.c
* merkt sich Syntaxen und deren Erfolg
* Kommt gar nicht mit geschachtelten Befehlen klar...
* \author Zesstra
*/

#pragma strong_types,save_types,rtt_checks
#pragma no_clone,no_inherit,no_shadow
#pragma pedantic, range_check

#include <defines.h>
#include <events.h>
#include <wizlevels.h>
#include <player/base.h>
#include <userinfo.h>
#include <driver_info.h>
#include <regexp.h>

#define HOME(x) (__PATH__(0)+x)
#define ZDEBUG(x) tell_room("/players/zesstra/workroom",\
                    sprintf("syntax: %O\n",x));

mapping blacklist;

struct cmd_s {
    string verb;
    string cmd;
    string uid;
    int success;
    int fp;
    int evalno;
    string tp_uid;
    int finished;
};

// LIste der letzten Kommandos, Queue zum speichern in die DB.
// Das letzte Element ist das letzte Kommando (ggf. noch nicht abgeschlossen)
private struct cmd_s *commands = ({});

protected void create()
{
  seteuid(getuid());
  if (sl_open(HOME("ARCH/syntaxen.sqlite")) != 1)
  {
    raise_error("Datenbank konnte nicht geoeffnet werden.\n");
  }
  // Tabellen und Indices anlegen, falls die nicht existieren.
  sl_exec("CREATE TABLE IF NOT EXISTS syntaxen("
          "cmd TEXT UNIQUE, "
          "verb TEXT NOT NULL, envuid TEXT NOT NULL, "
          "success INTEGER, count INTEGER, fp INTEGER);");
  sl_exec("CREATE INDEX IF NOT EXISTS idx_verb ON syntaxen(verb);");
  // dieses Objekt darf nur per hand (mit this_player()) geladen werden, weil
  // da ein Teil der Blacklist herkommt... Man beachte, dass dies auch das
  // ist, was die ueblichen Kommunikationsbefehle ausfiltert.
  string *tmp = map(this_player()->QueryProp(P_LOCALCMDS),
                    function string (mixed val)
                    {return val[0];}
                   );
  blacklist = mkmapping(tmp
                  - ({ "toete","schnupper", "schnuppere", "suche","such",
                       "unt", "untersuche","untersuch", "riech",
                       "rieche", "lausch", "lausche", "taste",
                       "fuehl", "fuehle, beruehre", "schau", "schaue",
                       "les", "lies", "betrachte", "betr", "betracht", 
                       "teile" // wird per regexp gefiltert!
                     })
                  + ({"mail","frage","frag", "xload", "xeval", "xcall",
                      "gruppe", "g", "team", "frufe", "fruf", "fwer",
                      "femote", "rknuddel", "knuddel", "nick", "nicke",
                      "rnick", "rnicke", "knuddel", "knuddele",
                      "rknuddel", "rknuddele","denke","denk",
                      "ffix","fnotiz","fuebertrage", "chaoskontrolle",
                      "kreis", // Matrixkristall
                     })
      );
}

private void write_db()
{
  foreach(struct cmd_s c : commands)
  {
    int** row=sl_exec("SELECT rowid, success, count, fp from syntaxen "
                      "WHERE cmd=?1;",
                      c->cmd);
    if (row)
    {
      sl_exec("UPDATE syntaxen SET success=?2, count=?3, fp=?4"
              "WHERE rowid=?1;", row[0][0],
              c->success || row[0][1],
              ++row[0][2], c->fp || row[0][3] );
    }
    else
    {
      sl_exec("INSERT INTO syntaxen(verb, cmd, envuid, success, count, fp) "
              "VALUES(?1,?2,?3,?4,?5,?6);",
              c->verb, c->cmd, c->uid, c->success, 1, c->fp);
    }
  }
  commands = ({});
}

// Beendet ein Kommando, wird nur intern gerufen
private void commit(struct cmd_s c)
{
  c->finished = 1;
  //printf("Kommando abgeschlossen: %O\n", c);
  if (get_eval_cost() > 1000000
      && sizeof(commands) > 5
      && find_call_out(#'write_db) == -1)
  {
    // In DB wegschreiben
    call_out(#'write_db, 1);
  }
}

public varargs int remove(int silent)
{
  write_db();
  destruct(this_object());
  return 1;
}

// gerufen, wenn ein Spieler ein Kommando eingetippt hat, aus modify_command()
// heraus. Nach Parsen und Kommandoblock, aber vor allem anderen.
// ACHTUNG: wenn ein Kommando erfolgreich ist, wird KEIN end_cmd() von aussen
// gerufen...
public void start_cmd(string cmdstr)
{
  if (!interactive(previous_object()))
      return;
  cmdstr = lower_case(cmdstr);
  // letztes Kommando notfalls abschliessen
  struct cmd_s cmd;
  if (sizeof(commands))
  {
    cmd = commands[<1];
    if (!cmd->finished)
    {
      // wir betrachten es uebrigens als erfolgreich!
      commit(cmd);
    }
  }
  // teile- mit und Ebenen und alle Kommandos, die nur aus verb und einem Wort
  // bestehen, sind auch unnuetz.
  //printf("%O\n",cmdstr);
  if (sizeof(explode(cmdstr, " ")) < 3
      || sizeof(cmdstr) > 100
      || regmatch(cmdstr,"^teile.*mit",RE_PCRE)
      || regmatch(cmdstr,"^-[[:alpha:]-]*[' :]{1}",RE_PCRE)
      || regmatch(cmdstr,"^[.']", RE_PCRE)
     )
  {
      return;
  }
  cmd = (<cmd_s> verb: explode(cmdstr, " ")[0],
         cmd: cmdstr,
         uid: getuid(environment(this_player())),
         evalno: driver_info(DI_EVAL_NUMBER),
         success: 1,
         tp_uid: getuid(previous_object()),
      );
  // sonstige Blacklist
  if (member(blacklist, cmd->verb))
      return;

  commands += ({cmd});
  //printf("Kommandostart: %O\n", cmd);
}

// gerufen von _auswerten() im Spielerobjekt - das kommt ziemlich am Ende der
// Kommandoverarbeitung - wenn es noch gerufen wird und uns ruft, betrachten
// wir das Kommando als NICHT erfolgreich.
public void cmd_unsuccessful()
{
  if (!sizeof(commands))
    return;
  struct cmd_s cmd = commands[<1];
  // Darf nur vom Spielerobjekt gerufen werden, welches das letzte Kommando
  // angefangen hat.
  if (cmd->tp_uid != getuid(previous_object()))
    return;
  // und dies gehoert nur zum letzten Kommando, wenn Verb und Eval-No
  // uebereinstimmen.
  if (cmd->verb == query_verb()
      || cmd->evalno == driver_info(DI_EVAL_NUMBER))
  {
    cmd->success = 0;
    //printf("Kommando nicht erfolgreich: %O\n", cmd);
    commit(cmd);
  }
  // Wenn nicht, war das letzte Kommando offenbar doch erfolgreich. Aus
  // irgendnem Grund haben wir aber den Start des neuen Kommandos verpasst.
  // Daher verwerfen wir das jetzt. Das alte Kommando war aber offenbar
  // erfolgreich, daher wird es jetzt abgeschlossen.
  else
  {
    commit(cmd);
  }
}

public void LogEP(int type)
{
  if (!sizeof(commands))
    return;
  struct cmd_s cmd = commands[<1];
  // Darf nur vom Spielerobjekt gerufen werden, welches das letzte Kommando
  // angefangen hat.
  if (cmd->tp_uid != getuid(previous_object()))
    return;
  //printf("FP gefunden: %O\n", cmd);
  // und dies gehoert nur zum letzten Kommando, wenn Verb und Eval-No
  // uebereinstimmen.
  if (cmd->verb == query_verb()
      || cmd->evalno == driver_info(DI_EVAL_NUMBER))
  {
    cmd->fp = type+1;
    // Und wenn es nen FP gab, ist das Kommando auch erfolgreich.
    commit(cmd);
  }
}

