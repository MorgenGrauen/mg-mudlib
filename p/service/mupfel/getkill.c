// /p/service/mupfel/getkill.c
// Ermittelt, wenn in 'die' aufgerufen, den Killer, auch wenn
// mit Bumerang, Kampfdaemon o.ae. Hilfsmitteln getoetet wurde
// 22-07-1996  - Mupfel@MorgenGrauen -
// Jetzt wird auch ein Beistand (Schutzengel, Phoenix, etc) erkannt
// 06-07-1997  - Mupfel@MorgenGrauen -
// Args der Pflanzenkrieger hat ja auch die id "Beistand" ;(
// 09-01-2001  - Mupfel@MorgenGrauen -
// Immer neue Retrowaffen... jetzt wird auch Tsunamis Toga erkannt
// 04-01-2005  - Mupfel@MorgenGrauen -
// 21.11.2006 - Zesstra
// Der Bumerang ist umgezogen. Pfad aktualisiert
// 13.10.2013 - Zesstra
// Einmal auf Basis von P_KILLER neugebaut.
// 16.10.2013 - Zesstra
// Man muss es jetzt nicht mehr erben, sondern kann auch per Callother
// abfragen. Dann wird der Killer vom previous_object() ermittelt.
// 12.06.2016 - Mupfel
// Zesstra war fleissig und hat diese Kruecke ueberfluessig gemacht.
// Siehe Manpage zu "get_killing_player"

#define NEED_PROTOTYPES
#include <thing/properties.h>
#undef NEED_PROTOTYPES

#include <thing/language.h>
#include <properties.h>

public mixed get_killer_data(int extern)
{
  object killer;
  // wenn von ausser gerufen und ein PO existiert, fragen wir P_KILLER darin
  // ab. Dann muss man dieses Ding hier nicht unbedingt erben.
  if ((extern || extern_call()) && previous_object())
      killer=previous_object()->QueryProp(P_KILLER);
  else
      killer=QueryProp(P_KILLER);

  // Eigentlich tritt dieser Fall nur bei Vergiftungen auf.
  if (!objectp(killer))
    return ({0,"<unbekannt>"});

  // Spieler selber? Ist ja einfach.
  if (query_once_interactive(killer))
    return ({killer,killer->Name(WER)});

  // Helfer-NPC?
  mixed helperdata = killer->QueryProp(P_HELPER_NPC);
  if (pointerp(helperdata) && objectp(helperdata[0]))
    return ({helperdata[0], helperdata[0]->Name(WER) + " mit Helfer-NPC"});
  // Einige Gilden-NPC verweisen auf ihren Spieler. Diese wird natuerlich nur
  // abgefragt, wenn die Gilde noch keinen Helfer-NPC registrieren.
  else if (killer->id("\nchaos_gilden_daemon"))
  {
    killer=find_player(killer->QueryProp("chaos_daem_beschwoerer"));
    return ({killer, killer->Name(WER)+" (mit Kampfdaemon)"});
  }
  else if (killer->id("Beistand") && !killer->id("P-KRIEGER"))
  {
    killer = killer->QueryProp("klerus_owner");
    return ({killer, killer->Name(WER)+" (mit Beistand)"});
  }

  // Einige toetende Objekte gesondert behandeln und Spieler als Killer
  // liefern.
  switch(object_name(blueprint(killer)))
  {
    case "/d/ebene/obj/gring":
      killer = killer->QueryProp(P_WORN);
      return ({killer, killer->name(RAW)+" (mit Goblinring)"});
    case "/d/unterwelt/wurzel/dorf/shabz":
      killer = killer->QueryProp(P_WORN);
      return ({killer, killer->name(RAW)+" (mit Abzeichen)"});
    case "/d/wueste/tsunami/schule/rue/hex_toga2":
      killer = killer->QueryProp(P_WORN);
      return ({killer, killer->name(RAW)+" (mit Toga)"});
    case "/p/seher/laden/obj/bumerang":
      killer = killer->query_werfer();
      return ({killer, killer->name(RAW)+" (mit Bumerang)"});
  }
  // killer ist jedenfalls kein Spieler hier.
  return ({killer, killer->name(RAW)});
}

// Name des Killers
public string get_killer()
{
  return get_killer_data(extern_call())[1];
}
// Objekt des Killers
public object get_killer_object()
{
  return get_killer_data(extern_call())[0];
}

