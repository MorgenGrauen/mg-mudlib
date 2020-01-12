#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <thing/language.h>
#include <thing/description.h>
#include <thing/commands.h>
#undef NEED_PROTOTYPES
#include <moving.h>
#include <items/kraeuter/kraeuter.h>
#include <defines.h>
#include <living/combat.h> // Fuer P_FREE_HANDS

public int AddPlantDetail(string filename);

// Standardwert von 2 h fuer vollstaendige Regeneration des Krautes. 
// Einfaches Nachwachsen ist typischerweise die Haelfte davon.
#define STD_WACHSTUM  7200
#define BS(x)         break_string(x, 78)

// Struktur (7 Eintraege pro Kraut):
struct plantinfo {
  int regrow;          // Zeit_bis_nachgewachsen
  int regeneration;    // Zeit_bis_komplett_regeneriert
  int pickable;        // per Standardkommanod pflueckbar?
  string *ids;         // Kraut-IDs
  string *adj;         // Kraut-Adjektive
  string name;         // Kraut->Name(WER,1)
  string *guards;       // ID_des_Bewacher-NPCs
};
// ([ filename : struct plantinfo ])
mapping plantMap = ([]);

// kann benutzt werden um schnell und einfach eine Pflanze in einem Raum
// hinzuzufuegen. Beispiel: AddPlant(BAERENKLAU);
// Diese Funktion erzeugt automatisch ein AddCmd() fuer das Pfluecken und
// (falls noch nicht vorhanden) Details fuer die Pflanze.
// Rueckgabewerte:
// 1 -> Erfolg; -1 -> filename ungueltig
varargs int AddPlant(string filename, string|string* npcId)
{
  // Eintrag und Details etc. hinzufuegen, wenn nicht erfolgreich, auch mit -1
  // beenden.
  if (AddPlantDetail(filename) == -1)
    return -1;

  struct plantinfo plant = plantMap[filename];

  // es ist pflueckbar
  plant->pickable = 1;

  // Bewacher noch eintragen
  if (npcId)
  {
    if (stringp(npcId))
      plant->guards = ({npcId});
    else
      plant->guards = npcId;
  }
  // Eine Befehlsfunktion brauchen wir dann natuerlich auch.
  AddCmd(({"pflueck", "pfluecke", "ernte"}), "_pfluecken");

  // erfolgreich fertig
 return 1;
}

// Wenn jemand per Hand das Plantdetail hinzufuegen moechte...
// z.B. bei Verwendung von GetPlant() anstelle von AddPlant()
// Im Gegensatz zu AddPlant() fuegt es kein Kommando zum Pfluecken hinzu und
// traegt keine Bewacher ein.
// 1 -> Erfolg; -1 -> filename ungueltig
public int AddPlantDetail(string filename)
{
  // Dateiname nicht uebergeben? Dann tun wir erstmal gar nix.
  if (!stringp(filename))
    return -1;
  object ob=load_object(filename);

  // Wenn wir zu dem Kraut schon Daten haben, werfen wir einen Fehler, damit
  // das beim Laden des Raumes schon erkannt wird.
  struct plantinfo plant = plantMap[filename];
  if (structp(plant))
    raise_error("AddPlant(): "+filename+" already exists.\n");

  plant = (<plantinfo>);
  // IDs und Adjektive parsen und den Datensatz zusammenstellen
  plant->ids = ob->Query(P_IDS, F_VALUE)-({ "Ding" });
  plant->adj = ob->Query(P_ADJECTIVES, F_VALUE);
  plant->name = ob->Name(WER,1);
  // Keine Bewacher
  plant->guards = ({});
  // Ausserdem ist es nicht pflueckbar per Standardkommando (pickable nicht
  // gesetzt)
  // Und eintragen
  plantMap[filename] = plant;

  // Details erzeugen aus Adjektiven und IDs
  string *name_adj = ob->QueryProp(P_NAME_ADJ);

  // aktuelles Geschlecht zwischenspeichern, wird spaeter wiederhergestellt
  int gender = Query(P_GENDER, F_VALUE);
  Set(P_GENDER, ob->Query(P_GENDER, F_VALUE), F_VALUE);

  // erzeugt fuer jede moegliche Kombination aus Adjektiv im Akkusativ
  // und ID des Krautes ein Detail.
  name_adj = map(name_adj, #'DeclAdj, WEN, 0);

  // Geschlecht zuruecksetzen
  Set(P_GENDER, gender, F_VALUE);

  string *det=({});
  foreach(string _id : plant->ids) {
    foreach(string _adj : name_adj) {
      det += ({ _adj + _id });
    }
  }

  det += plant->ids;
  // keine existierenden Details ueberschreiben
  det -= m_indices(Query(P_DETAILS, F_VALUE) || ([]));
  if (sizeof(det))
    AddDetail(det, ob->Query(PLANT_ROOMDETAIL, F_VALUE));

  return 1;
}

// Prueft, ob die Pflanze zu "filename" in diesem Raum schon nachgewachsen
// ist.
protected int CheckPlant(string filename)
{
  struct plantinfo plant = plantMap[filename];
  // Wenn es keinen Eintrag gibt, gibt es offenbar keine Pflanze
  if (!structp(plant))
    return 0;

  // Solange die Zeit arr[0] noch nicht erreicht ist, ist das Kraut nicht 
  // nachgewachsen, dann gibt es gar nix.
  return (time() > plant->regrow);
}

// Moechte man AddPlant() nicht benutzen, weil man die Pflanze nicht einfach
// pfluecken, sondern vielleicht abschneiden, oder ausgraben soll, so kann
// man sich mittels GetPlant(filename) das Objekt erzeugen lassen. Gibt
// GetPlant() 0 zurueck, ist die Pflanze noch nicht wieder weit genug
// nachgewachsen.
object GetPlant(string filename)
{
  struct plantinfo plant = plantMap[filename];
  // Wenn es keinen Eintrag gibt, gibt es offenbar keine Pflanze
  if (!structp(plant))
    return 0;

  // regrow enthaelt den Zeitpunkt, wann das Kraut nachgewachsen ist,
  // regeneration denjenigen, wann es vollstaendig regeneriert ist.

  // Vor dem Nachgewachsen-Zeitpunkt kann man gar nix ernten.
  if (time() < plant->regrow)
    return 0; // noch nicht nachgewachsen

  // Restzeit bis zur vollstaendigen Regeneration.
  int regeneriert = plant->regeneration - time();

  // Wenn vollstaendig regeneriert, ist STD_WACHSTUM die neue Zeit bis zur
  // Regeneration. Wenn noch nicht vollstaendig regeneriert, Restzeit
  // verdoppeln und STD_WACHSTUM nochmal drauf addieren.
  if (regeneriert<=0)
    regeneriert = STD_WACHSTUM;
  else
    regeneriert = (regeneriert*2)+STD_WACHSTUM;

  // Zeitpunkt des Nachwachsen ist die halbe Regenerationszeit
  plant->regrow = time() + (regeneriert/2);
  // Zeitpunkt voelliger Regeneration
  plant->regeneration = regeneriert + time();

  return clone_object(filename);
}

static int _pfluecken(string str)
{
  int res;

  if (!mappingp(plantMap)) return 0;
  _notify_fail("WAS moechtest Du pfluecken?\n");

  // IDs und Adjektive zwischenspeichern
  mixed ids = Query(P_IDS, F_VALUE);
  mixed adj = Query(P_ADJECTIVES, F_VALUE);

  foreach(string key, struct plantinfo plant : plantMap) 
  {
    if ( !structp(plant) || !plant->pickable )
      continue;

    // IDs und Adjektive des Krautes kopieren 
    Set(P_IDS, plant->ids, F_VALUE);
    Set(P_ADJECTIVES, plant->adj, F_VALUE);

    // Syntaxpruefung wird dann mit id() gemacht.
    if (id(str))
    {
      object ob;
      object bewacher;
      res=1;

      // Liste der eingetragenen Bewacher-IDs durchlaufen und pruefen, ob
      // mindestens einer davon anwesend ist.
      foreach( string npcId : plant->guards )
      {
        bewacher = present(npcId, ME);
        if (objectp(bewacher))
          break;
      }

      if ( !PL->QueryProp(P_FREE_HANDS) )
      {
        tell_object(PL, BS("Du hast keine Hand frei, um etwas pfluecken "
          "zu koennen."));
      }
      // Ist der Bewacher anwesend? Dann kann man das Kraut nicht pfluecken.
      else if ( objectp(bewacher) )
      {
        tell_object(PL, BS(bewacher->Name(WER, 2)+" laesst Dich "
          "leider nicht nah genug heran. Irgendwie musst Du Dich wohl "
          "zunaechst um "+bewacher->QueryPronoun(WEN)+" kuemmern."));
      }
      // Wenn GetPlant() ein Objekt liefert, kann was gepflueckt werden.
      else if ( objectp(ob=GetPlant(key)) )
      {
        if ( ob->move(PL, M_GET) == MOVE_OK )
        {
          write(BS("Vorsichtig pflueckst Du "+ob->name(WEN, 1)+
            " und nimmst "+ob->QueryPronoun(WEN)+" an Dich."));
        }
        else 
        {
          write(BS("Vorsichtig pflueckst Du "+ob->name(WEN, 1)+", kannst "+
            ob->QueryPronoun(WEN)+" aber nicht nehmen."));
          ob->move(environment(PL), M_GET);
        }
      }
      // Wenn alles nicht, dann ist das Kraut noch nicht wieder da.
      else 
      {
        write(BS(plant->name+" ist noch nicht reif genug "
          +"und muss erst noch etwas wachsen."));
        break;
      }
    }
  }
   // IDs und Adjektive zuruecksetzen.
  Set(P_IDS, ids, F_VALUE);
  Set(P_ADJECTIVES, adj, F_VALUE);

  return res;
}

