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

// Standardwert von 2 h fuer vollstaendige Regeneration des Krautes. 
// Einfaches Nachwachsen ist typischerweise die Haelfte davon.
#define STD_WACHSTUM  7200
#define BS(x)         break_string(x, 78)

// Struktur (6 Eintraege pro Kraut):
// ([ filename : ({ Zeit_bis_nachgewachsen, Zeit_bis_komplett_regeneriert,
//                  Kraut-IDs, Kraut-Adjektive, Kraut->Name(WER,1),
//                  ID_des_Bewacher-NPCs }) ])
mapping plantMap = ([]);

// kann benutzt werden um schnell und einfach eine Pflanze in einem Raum
// hinzuzufuegen. Beispiel: AddPlant(BAERENKLAU);
// Diese Funktion erzeugt automatisch ein AddCmd() fuer das Pfluecken und
// (falls noch nicht vorhanden) Details fuer die Pflanze.
// Rueckgabewerte:
// 1 -> Erfolg; -1 -> filename ungueltig
varargs int AddPlant(string filename, string|string* npcId)
{
  mixed arr;

  // Dateiname nicht uebergeben? Dann tun wir erstmal gar nix.
  if (!stringp(filename)) 
    return -1;
  object ob=load_object(filename);
   
  // Wenn wir zu dem Kraut schon Daten haben (erkennbar an >2 Eintraegen),
  // werfen wir einen Fehler, damit das beim Laden des Raumes schon
  // erkannt wird.
  if (pointerp(arr=plantMap[filename]) && sizeof(arr)>2)
    raise_error("AddPlant(): "+filename+" already exists.\n");

  // IDs und Adjektive parsen und den Datensatz zusammenstellen
  string *ids = ob->Query(P_IDS, F_VALUE)-({ "Ding" });
  string *adj = ob->Query(P_ADJECTIVES, F_VALUE);
  
  if (!pointerp(arr) || sizeof(arr)<2) 
    arr = ({0,0});
  if ( !npcId )
    npcId = ({});
  else if (stringp(npcId))
    npcId = ({npcId});
  plantMap[filename]=({arr[0], arr[1], ids, adj, ob->Name(WER, 1), npcId });
   
  // Details erzeugen aus Adjektiven und IDs
  adj = ob->QueryProp(P_NAME_ADJ);
  
  // aktuelles Geschlecht zwischenspeichern, wird spaeter wiederhergestellt
  int gender = Query(P_GENDER, F_VALUE);
  Set(P_GENDER, ob->Query(P_GENDER, F_VALUE), F_VALUE);
  
  // erzeugt fuer jede moegliche Kombination aus Adjektiv im Akkusativ
  // und ID des Krautes ein Detail.
  adj = map(adj, #'DeclAdj, WEN, 0);

  string *det=({});
  foreach(string _id : ids) {
    foreach(string _adj : adj) {
      det += ({ _adj + _id });
    }
  }

  det += ids;
  // keine existierenden Details ueberschreiben
  det -= m_indices(Query(P_DETAILS, F_VALUE) || ([]));
  if (sizeof(det)) 
    AddDetail(det, ob->Query(PLANT_ROOMDETAIL, F_VALUE));
  
  // Eine Befehlsfunktion brauchen wir natuerlich auch.
  AddCmd(({"pflueck", "pfluecke", "ernte"}), "_pfluecken");
  
  return 1;
}

// Wenn jemand per Hand das Plantdetail hinzufuegen moechte...
// z.B. bei Verwendung von GetPlant() anstelle von AddPlant()
void AddPlantDetail(string filename)
{
  // Pfad in Objektpointer wandeln
  object ob=load_object(filename);
   
  // Details erzeugen
  string *det = ({});
  string *ids = ob->Query(P_IDS, F_VALUE)-({ "Ding" });
  string *adj = ob->QueryProp(P_NAME_ADJ);
  // aktuelles Geschlecht zwischenspeichern, wird spaeter wiederhergestellt
  int gender=Query(P_GENDER, F_VALUE);
  Set(P_GENDER, ob->Query(P_GENDER, F_VALUE));
  // erzeugt fuer jede moegliche Kombination aus Adjektiv im Akkusativ
  // und ID des Krautes ein Detail.
  adj = map(adj, #'DeclAdj, WEN, 0);
  foreach(string _id : ids) {
    foreach(string _adj : adj) {
      det += ({ _adj + _id });
    }
  }
  AddDetail(det+ids, ob->Query(PLANT_ROOMDETAIL, F_VALUE));
  // Geschlecht zuruecksetzen
  Set(P_GENDER, gender, F_VALUE);
}

// Prueft, ob die Pflanze zu "filename" in diesem Raum schon nachgewachsen
// ist.
protected int CheckPlant(string filename)
{
  mixed arr=plantMap[filename];
  if (!pointerp(arr) || sizeof(arr)<2) {
    arr=plantMap[filename]=({ 0, 0 });
  }
  // Solange die Zeit arr[0] noch nicht erreicht ist, ist das Kraut nicht 
  // nachgewachsen, dann gibt es gar nix.
  return (time()>arr[0]);
}

// Moechte man AddPlant() nicht benutzen, weil man die Pflanze nicht einfach
// pfluecken, sondern vielleicht abschneiden, oder ausgraben soll, so kann
// man sich mittels GetPlant(filename) das Objekt erzeugen lassen. Gibt
// GetPlant() 0 zurueck, ist die Pflanze noch nicht wieder weit genug
// nachgewachsen.
object GetPlant(string filename)
{
  int *arr=plantMap[filename];
  if (!pointerp(arr) || sizeof(arr)<2)
  {
     arr=plantMap[filename]=({ 0, 0 });
  }
  // arr[0] enthaelt den Zeitpunkt, wann das Kraut nachgewachsen ist,
  int nachgewachsen = arr[0];
  // arr[1] denjenigen, wann es vollstaendig regeneriert ist.
  int regeneriert = arr[1];

  // Vor dem Nachgewachsen-Zeitpunkt kann man gar nix ernten.
  if (time()<nachgewachsen) return 0; // noch nicht nachgewachsen

  // Restzeit bis zur vollstaendigen Regeneration.
  regeneriert-=time();
  
  // Wenn vollstaendig regeneriert, ist STD_WACHSTUM die neue Zeit bis zur
  // Regeneration. Wenn noch nicht vollstaendig regenriert, Restzeit
  // verdoppeln und STD_WACHSTUM nochmal drauf addieren.
  regeneriert = (regeneriert<=0 ? STD_WACHSTUM 
                                : (regeneriert*2)+STD_WACHSTUM);
  // nachgewachsen ist die halbe Regenerationszeit
  arr[0]=nachgewachsen=time()+(regeneriert/2);
  // Zeit voelliger Regeneration
  arr[1]=regeneriert+=time();
  
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

  foreach(string key, mixed krautinfo : plantMap) 
  {
    if ( sizeof(krautinfo) != 6 )
      continue;
 
    // IDs und Adjektive des Krautes kopieren 
    Set(P_IDS, krautinfo[2], F_VALUE);
    Set(P_ADJECTIVES, krautinfo[3], F_VALUE);

    // Syntaxpruefung wird dann mit id() gemacht.
    if (id(str)) 
    {
      object ob;
      object bewacher;
      res=1;

      // Liste der eingetragenen Bewacher-IDs durchlaufen und pruefen, ob
      // mindestens einer davon anwesend ist.
      foreach( string npcId : krautinfo[5] )
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
        write(BS(krautinfo[4]+" ist noch nicht reif genug "
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

