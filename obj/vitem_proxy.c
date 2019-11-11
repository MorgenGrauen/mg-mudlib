#pragma strong_types,rtt_checks

inherit "/std/container";
#include <moving.h>
#include <errord.h>
#include <config.h>
#include <language.h>
#include <living/comm.h>

object cloner;

protected void create()
{
  cloner=previous_object();
  ::create();
  // Das Objekt raeumt sich selber im reset auf. (D.h. kein Env-Check)
  SetAutoObject(1);
}

// Dieses Objekt raeumt sich immer im ersten reset auf - es ist kurzlebig und
// transient und Aenderungen an seinen Daten bleiben nicht erhalten.
void reset()
{
  remove(1);
}

public varargs void init(object origin)
{
  // leer ueberschreiben. Das klingt abstrus, hat aber folgenden Hintergrund:
  // dieses Objekt ist nicht dazu gedacht, in irgendeine Umgebung zu gelangen.
  // Und selbst wenn das jemand mit M_NOCHECK tut, soll die Interaktion dieses
  // Objekts mit anderen minimiert werden. Also keine Anmeldung von actions
  // etc.
}

// Auch dies ist ungewoehnlich: Bewegung nie erlauben, daher eruebrigt sich
// der Aufruf des geerbten PreventMove(). Auch wird M_NOCHECK nicht
// beruecksichtigt... Don't do this anywhere else.
protected int PreventMove(object dest, object oldenv, int method)
{
  return ME_NOT_ALLOWED;
}

public object *AllVirtualEnvironments()
{
  if (cloner)
  {
    object *cloner_envs = all_environment(cloner)
                          || cloner->AllVirtualEnvironments();
    if (cloner_envs)
      return ({cloner}) + cloner_envs;
    return ({cloner});
  }
  return 0;
}

// Fehler werden nicht an diesem Objekt eingetragen, sondern am erzeugenden
// Objekt. Wenn das unbekannt ist, wird das Standardverfahren zum Logging
// verwendet.
public int SmartLog(string unused, string error_type, string meldung,
                    string date)
{
  // Wenn kein Cloner, loggt der Aufrufer (vermutlich an diesem Objekt).
  // Und ausserdem nur Meldungen von Spielershells akzeptieren.
  if (!cloner
      || strstr(load_name(previous_object()),"/std/shells/") == -1)
    return 0;

  mapping err = ([ F_PROG: "unbekannt",
           F_LINE: 0,
           F_MSG: meldung,
           F_OBJ: cloner
         ]);
  string desc="etwas unbekanntes";
  switch(error_type) {
    case "BUGS":
      desc="einen Fehler";
      err[F_TYPE]=T_REPORTED_ERR;
      break;
    case "DETAILS":
      desc="ein fehlendes Detail";
      err[F_TYPE]=T_REPORTED_MD;
      break;
    case "IDEA":
      desc="eine Idee";
      err[F_TYPE]=T_REPORTED_IDEA;
      break;
    case "TYPO":
      desc="einen Typo";
      err[F_TYPE]=T_REPORTED_TYPO;
      break;
    case "SYNTAX":
      desc="einen Syntaxhinweis";
      err[F_TYPE]=T_REPORTED_SYNTAX;
      break;
  }
  // Eintragung in die Fehler-DB
  string hashkey = (string)ERRORD->LogReportedError(err);
  if (stringp(hashkey))
  {
    previous_object()->ReceiveMsg(sprintf(
          "Ein kleiner Fehlerteufel hat D%s an %s unter der ID %s "
          "notiert.",
          (cloner->IsRoom() ? "diesem Raum" : cloner->name(WEM,1)), desc,
          hashkey || "N/A"),
        MT_NOTIFICATION | MSG_DONT_BUFFER | MSG_DONT_STORE | MSG_DONT_IGNORE,
        MA_UNKNOWN, 0, this_object());
    return 1; // wurde erfolgreich protokolliert.
  }

  return 0; // nicht erfolgreich, Aufrufer muss protokollieren.
}
