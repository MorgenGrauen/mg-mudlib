//---------------------------------------------------------------------------
// Name des Objects:    Schluesselbrett
// Letzte Aenderung:    02.06.2001
// Original:            Swift
// Neue Version:        Seleven
// Übernommen:          23.01.2006
//---------------------------------------------------------------------------
/* Changelog
   * 21.06.2007, Zesstra
     im inherit abs. Pfad durch LADEN(x) ersetzt.
   * 01.07.2007, Zesstra
     GetOwner() definiert.
   * 11.05.2015, Arathorn
     weitraeumig ueberarbeitet zur Behebung von Fehlfunktionen
*/
#pragma strong_types, rtt_checks

#include "/d/seher/haeuser/moebel/schrankladen.h"
#include <ansi.h>
inherit  LADEN("swift_std_container");

#define VERSION_OBJ "3"
// Zur Indizierung des Mappings <haken>
#define HOOK_OB    0
#define HOOK_LABEL 1

private varargs string KeyDescription(int keynum);
private int IsValidKey(object ob);
private int GetNextFreeSlot();
private object GetKeyForLabel(string label);
private int GetHookNumberForKey(object ob);
private void ConsolidateInventory();

// Struktur: ([ haken_nr : objekt; beschriftung ])
private mapping haken=([:2]);
private int hooklist_long = 0;

protected void create() {
  if (!clonep(TO)) {
    set_next_reset(-1);
    return;
  }
  ::create();
  
  foreach(int i: 1..16) {
    m_add(haken, i);
  }

  // DEBUG! Sonst funktioniert's ausserhalb von Seherhaeusern nicht.
  SetProp("test",1);
  SetProp(P_SHORT, "Ein Schluesselbrett");
  Set(P_LONG, function string () {
    string text = BS(
      "Ein kleines Brett aus Holz, an dem in 2 Reihen je 8 Haken befestigt "
      "sind, an die man insgesamt 16 Schluessel haengen kann. Um die "
      "Uebersicht nicht zu verlieren, befindet sich ueber jedem der Haken "
      "ein kleines Schildchen aus Schiefer, das sich beschriften laesst. "
      "Auf diese Weise geht der Sinn und Zweck des Schluessels, der an "
      "diesem Haken ruht, nicht verloren. Praktischerweise haengt an einer "
      "Schnur ein Stueck Kreide von dem Brett herab, mit dem Du die "
      "Schildchen beschriften kannst.");
    text += KeyDescription(0);
    return text; 
  }, F_QUERY_METHOD);

  AddId(({"schluesselbrett","brett"}));
  SetProp("cnt_version_obj", VERSION_OBJ);
  SetProp(P_CNT_STATUS, 0);
  SetProp(P_NAME, "Schluesselbrett");
  SetProp(P_GENDER, NEUTER);
  SetProp(P_WEIGHT, 1000);         // Gewicht 1 Kg
  SetProp(P_MAX_WEIGHT, 1000000);  // Es passen fuer 1000 kg Sachen rein.
  SetProp(P_WEIGHT_PERCENT, 100);  // Dafuer macht er auch nix leichter :)
  SetProp(P_MAX_OBJECTS, 16);
  // Man soll nichts ueber den normalen Mechanismus reinstecken oder 
  // rausnehmen koennen. Wird von put_and_get.c geprueft.
  SetProp(P_CONTAINER, 0);
  SetProp(P_NOINSERT_MSG, BS("An das Schluesselbrett kannst Du nur "
    "Schluessel haengen.\nSyntax: 'haenge <schluessel> an brett'."));
  SetProp(P_NOGET, "Das geht nicht. "+Name(WER,1)+
    " laesst sich nicht bewegen.\n");
  SetProp(P_MATERIAL, ({MAT_OAK, MAT_MISC_MAGIC}) );
  SetProp(P_INFO, "Versuch's mal mit: \"skommandos schluesselbrett\".\n");

  AD(({"holz"}), BS("Echte, robuste Eiche."));

  RemoveCmd(({"schliesse","schliess","oeffne"}));

  AddCmd(({"unt", "untersuche", "betrachte", "betracht", "betr",
           "schaue", "schau"}), "unt_haken");
  AddCmd(({"haeng", "haenge"}), "schluessel_an_haken_haengen");
  AddCmd(({"nimm"}), "schluessel_von_haken_nehmen");
  AddCmd("sbeschrifte", "cmd_beschriften");
  AddCmd("sliste", "cmd_sliste");
}

/***** KOMMANDOFUNKTIONEN *****/

// Konfiguration der Schluesselliste, entweder "alle" anzeigen oder nur die
// belegten Haken.
static int cmd_sliste(string str) {
  notify_fail("Syntax: 'sliste alle|belegte'.\n");
  if ( !stringp(str) || !sizeof(str) )
    return 0;

  if ( str == "alle" ) {
    if ( !hooklist_long ) {
      tell_object(TP, BS("Ok, es werden jetzt immer alle Haken "
        "aufgelistet."));
      hooklist_long = 1;
    }
    else 
      tell_object(TP, BS("Die Einstellung war schon 'alle'."));
  }
  else if ( str == "belegte" ) {
    if ( hooklist_long ) {
      tell_object(TP, BS("Ok, es werden jetzt nur die belegten Haken "
        "aufgelistet."));
      hooklist_long =0;
    }
    else 
      tell_object(TP, BS("Die Einstellung war schon 'belegte'."));
  }
  else 
    return 0;
  return 1;
}

// Haken untersuchen. Ohne Angabe der Nummer werden alle ausgegeben,
// ansonsten der gewuenschte, sofern es diesen gibt.
static int unt_haken(string str) {
  if ( !stringp(str) || !sizeof(str) || strstr(str, "haken") == -1 )
    return 0;
  
  int hakennr;
  if ( sscanf(str, "haken %d", hakennr) != 1 ) {
    string det = KeyDescription(hakennr);
    tell_object(TP, sizeof(det) ? det : "Die Haken sind alle leer.\n");
    return 1;
  }

  notify_fail("Welchen Haken moechtest Du untersuchen? Es gibt nur die "
    "Haken 1 bis 16.\n");
  if( hakennr<1 || hakennr > 16 )
    return 0;

//  ausgabe_haken(hakennr);
  tell_object(TP, KeyDescription(hakennr));
  return 1;
}

// Schluessel aufhaengen.
static int schluessel_an_haken_haengen(string str) {
  notify_fail("Was willst Du woran haengen?\n");
  if(!stringp(str) || !sizeof(str))
    return 0;
  
  notify_fail("Das darfst Du nicht.\n");
  if(!erlaubt())
    return 0;

  string was, woran;
  notify_fail("Folgende Syntax versteht Dein Schluesselbrett:\n"
    "  \"haenge <schluesselname> an haken [nr]\" oder\n"
    "  \"haenge <schluesselname> an [schluessel]brett\".\n");
  if ( sscanf(str, "%s an %s", was, woran) != 2 )
    return 0;

  notify_fail("Was moechtest Du an das Schluesselbrett haengen?\n");
  if ( !stringp(was) ) 
    return 0;

  notify_fail("Woran willst Du das denn haengen?\n");
  if ( strstr(woran, "haken", 0)==-1 && !id(woran) )
    return 0;

  // Test, ob es Objekte dieses Namens gibt.
  object *obs = TP->find_obs(was, PUT_GET_NONE);
  // Aus der Liste der gefundenen Objekte alle rausfiltern, die keine
  // Schluessel sind. Als Schluessel gelten hier alle Objekte, die die
  // ID "schluessel" haben oder die mittels QueryDoorKey() einer bestimmten
  // Tuer zugeordnet sind.
  obs = filter(obs, #'IsValidKey);
  notify_fail("Keine(n) Schluessel mit dem Namen gefunden.\n");
  if ( !sizeof(obs) )
    return 0;

  int nr = GetNextFreeSlot();
  notify_fail("Am Schluesselbrett ist kein Haken mehr frei.\n");
  if ( !nr )
    return 0;

  // Wenn nur ein Schluessel gefunden wurde, wird genauer geschaut, ob
  // der Spieler gesagt hat, wo der hinsoll.
  if ( sizeof(obs) == 1 ) {
    if ( woran != "haken" || !id(woran) )
      sscanf(woran, "haken %d", nr);
  }

  notify_fail("Es gibt nur die Haken 1 bis 16.\n");
  if ( nr < 1 || nr > 16 )
    return 0;

  notify_fail("Der Haken ist nicht mehr frei.\n");
  if ( objectp(haken[nr,HOOK_OB]) )
    return 0;

  // Alle gefundenen Schluesselobjekte ans Brett haengen.
  foreach( object key : obs ) {
    switch (key->move(ME, M_PUT)) {
      // Die Kapazitaet ist ueber P_MAX_OBJECTS begrenzt. Alle
      // Schleifendurchlaeufe, die die Kapazitaetsgrenze erreichen, landen
      // in default und beenden damit die Schleife. Daher muessen sie 
      // nicht gesondert behandelt werden.
      case MOVE_OK :
        // Falls das Brett voll sein sollte, aber faelschlicherweise 
        // dennoch ein weiterer Schluessel hineinbewegt wurde, ist nr==0,
        // und es wird ein Fehler ausgeloest, damit nicht hinterher ein
        // Schluessel an Haken 0 haengt.
        if ( !nr ) {
          raise_error("Fehler: Haken 0 darf nicht belegt werden.");
          break;
        }
        tell_object(TP, BS("Du haengst "+obs[0]->name(WEN,1)+" an Haken "+
          nr+"."));
        // Nur wenn ein Schluessel erfolgreich drangehaengt wurde, wird er
        // in die Liste eingetragen. Etwaige Beschriftungen bleiben bestehen,
        haken[nr, HOOK_OB] = key;
        // und nur dann wird der naechste Slot probiert. Wenn nur ein 
        // Schluessel verarbeitet wird, ist das hier unschaedlich, weil es 
        // nicht nochmal ausgewertet wird.
        nr = GetNextFreeSlot();
        // Schluessel wurde verarbeitet und Meldung ausgegeben
        obs -= ({key});
        break;
      case ME_CANT_LEAVE_ENV :
      case ME_CANT_BE_INSERTED :
      case ME_CANT_BE_DROPPED :
        // Schluessel wurde nicht erfolgreich drangehaengt.
        tell_object(TP, BS(key->Name(WER,1)+" weigert sich, ans "
          "Schluesselbrett gehaengt zu werden."));
        // Meldung wurde ausgegeben, nichts weiter zu tun.
        obs -= ({key});
        break;
      // TOO_MANY_OBJECTS wird nicht hier behandelt, sondern weiter unten,
      // um eine Auflistung der Schluesselnamen ausgeben zu koennen, statt
      // eine Zeile pro Schluessel. Die anderen Rueckgabewerte von move() 
      // sind bei dieser Art der Bewegung nicht zu erwarten.
      default: break;
    }
  }
  // Fuer alle uebrigen war wohl TOO_MANY_OBJECTS der Rueckgabewert, denn
  // das Schluesselbrett erlaubt nur P_MAX_OBJECTS = 16.
  if ( sizeof(obs) ) {
    tell_object(TP, BS("Fuer "+CountUp(obs->name(WEN))+" ist leider kein "
      "Haken mehr am Schluesselbrett frei."));
  }
  return 1;
}

// Schluessel abnehmen
static int schluessel_von_haken_nehmen(string str) {
  if( !stringp(str) || !sizeof(str) ) 
    return 0;

  string was, woraus;
  if ( sscanf(str, "%s aus %s", was, woraus) != 2 && 
       sscanf(str, "%s von %s", was, woraus) != 2 )
    return 0;

  notify_fail("Das darfst Du nicht!\n");
  if(!erlaubt()) 
    return 0;
 
  notify_fail("Woraus willst Du das denn nehmen?\n");
  if ( !id(woraus) && strstr(woraus, "haken", 0) == -1 ) 
    return 0;

  notify_fail("Was willst Du denn vom Schluesselbrett nehmen?\n");
  if( !stringp(was) ) 
    return 0;
  
  notify_fail("Das Schluesselbrett ist doch aber leer.\n");
  if ( !sizeof(all_inventory(ME)) )
    return 0;

  int hakennr;
  object *keys = ({});
  // Es muessen auch Schluessel von bestimmten Haken abgenommen
  // werden koennen, nicht nur jeweils der erste mit der angegebenen ID.
  // Dies hat auch Prio gegenueber der allgemeinen Begriffssuche mit
  // find_obs().
  if ( sscanf(woraus, "haken %d", hakennr) == 1 ) {
    notify_fail("Es gibt nur die Haken 1 bis 16.\n");
    if ( hakennr < 1 || hakennr > 16 ) 
      return 0;
    notify_fail("An diesem Haken haengt aber doch kein Schluessel.\n");
    if ( !objectp(haken[hakennr,HOOK_OB]) )
      return 0;
    
    keys += ({ haken[hakennr,HOOK_OB] });
  }

  // Keiner gefunden? Vielleicht ist ja einer mit einer bestimmten 
  // Beschriftung gemeint?
  if ( !sizeof(keys) ) {
    // Schluessel mit dem angegebenen Namen suchen
    keys = TP->find_obs(was+" in brett in raum");
    // Hakennummer ermitteln, wenn eine Beschriftung angegeben wurde.
    string wanted_desc;
    sscanf(was, "schluessel fuer %s", wanted_desc);
    object wanted_key = GetKeyForLabel(wanted_desc);
    if ( objectp(wanted_key) )
      keys += ({wanted_key});
  }
  // Immer noch nix gefunden? Dann muss sich der Spieler was anderes 
  // einfallen lassen.
  notify_fail("Am Schluesselbrett haengt kein solcher Schluessel.\n");
  if ( !sizeof(keys) )
    return 0;

  // Alle gefundenen Schluessel verarbeiten. Alle, die nicht bewegt werden
  // koennen, verbleiben in der Liste.
  foreach( object key : keys ) {
    int current_hook = GetHookNumberForKey(key);
    if ( key->move(TP,M_GET) == MOVE_OK ) {
      keys -= ({ key });
      tell_object(TP, BS("Du nimmst "+key->name(WEN,1)+" von Haken "+
        current_hook+" ab."));
      // Schluessel austragen; bestehende Beschriftungen bleiben erhalten
      haken[current_hook, HOOK_OB] = 0;
    }
  }
  // Von allen nicht vom Haken genommenen Schluesseln die Namen ermitteln
  // und auflisten.
  if ( sizeof(keys) ) {
    tell_object(TP, BS(capitalize(CountUp(keys->name(WER,1)))+" "+
      (sizeof(keys)==1?"konnte":"konnten")+" nicht vom Schluesselbrett "
      "abgenommen werden."));
  }
  return 1;
}

// Verfuegbare Kommandos auflisten.
varargs int skommandos(string str) {
  notify_fail( "Fehler: Dieser Befehl benoetigt eine gueltige Objekt-Id als "
    +"Parameter.\nBeispiel: skommandos [schluessel]brett\n");
  
  if( !id(str) ) return 0;

  write(
    "==================================================================="
    "==========\n"
    "Aktuelle Version: "+QueryProp("cnt_version_std")+
      QueryProp("cnt_version_obj")+"\n"+
    BS(Name(WER,1)+" kann nur in diesem Seherhaus verwendet werden, "
    "da "+QueryPronoun(WER)+" speziell dafuer gekauft wurde. "+
    CAP(QueryPronoun(WER))+" verfuegt ueber folgende Kommandos:")+
    "----------------------------------------------------------------------"
    "-------\n"
    "serlaube [schluessel]brett [spielername|\"hausfreunde\"|\"familie\"]\n"
    "  Erlaubt Personen, "+name(WEN,1)+" mitzubenutzen.\n"
    "  serlaube ohne Angabe eines Arguments listet alle Personen mit Zugriff "
      "auf\n  "+name(WEN,1)+" auf.\n\n"
    "verschiebe [schluessel]brett nach [ausgang]\n"
    "  Damit kannst Du "+name(WEN,1)+" innerhalb Deines Seherhauses "
    "verschieben.\n\n"
    "sverstecke [schluessel]brett\n"
    "  Damit machst Du "+name(WEN,1)+" unsichtbar.\n"
    "shole [schluessel]brett hervor\n"
    "  Damit machst Du "+name(WEN,1)+" wieder sichtbar.\n"
    "zertruemmer [schluessel]brett\n"
    "  Damit zerstoerst Du das Schluesselbrett.\n"
    "sbeschrifte haken <nr> mit <text>|nichts\n"
    "  Eine Beschriftung an einem Haken anbringen oder loeschen.\n"
    "sliste lang|kurz\n"
    "  Schluesselanzeige konfigurieren: alle oder nur belegte auflisten.\n"
    "haenge <schluesselname> an haken [nr]\n"
    "haenge <schluesselname> an [schluessel]brett\n"
    "  Damit haengst Du einen Schluessel an das Brett.\n"
    "nimm schluessel von haken <nr>\n"
    "nimm <schluesselname> von [schluessel]brett\n"
    "nimm schluessel fuer <beschriftung> von [schluessel]brett.\n"
    "  Damit nimmst Du einen Schluessel vom Schluesselbrett.\n"
    "==================================================================="
    "==========\n");
  return 1;
}

static int erlaubnis_liste()
{
  if( QueryHausbesitzer() != QueryTP() && !QueryProp("test") )
  {
    tell_object(TP, BS("Nur "+QueryHausbesitzer()+" darf Berechtigungen "
      +name(WESSEN,1)+" abfragen!"));
    return 1;
  }
  
  string *strs;
  string str = 
    "==================================================================="
    "==========\n";
  if ( hauserlaubnis ) {
    strs = SHVERWALTER->HausProp(LOWER(QueryHausbesitzer()), 2);
    str += "Folgende Freunde Deines Hauses duerfen "+name(WEN,1)+
      " mitbenutzen:\n";
    if(sizeof(strs)) {
      str += BS(CountUp(strs));
    }
    else {
      str += BS("Du hast keiner Person Zugriff auf Dein Haus gewaehrt...");
    }
  }
  else {
    str+= BS("Die Freunde Deines Hauses duerfen "+name(WEN,1)+
      " nicht mitbenutzen.");
  }

  str += 
    "-------------------------------------------------------------------"
    "----------\n";

  if(familie)
  {
  if( "/secure/zweities"->QueryFamilie(TP) == familie)
    str+=BS("Alle Deine Familienmitglieder duerfen "
             +name(WEN,1)+" oeffnen/schliessen.");
  else
    str+=BS( "Alle Familienmitglieder von "+CAP(explode(familie,"_")[0])
             +" duerfen " + name(WEN,1) + " oeffnen/schliessen.");
  str+="-------------------------------------------------------------------"
        "----------\n";
  }

  strs=QueryProp("cnt_erlaubnis");

  if(sizeof(strs)) {
    str += BS("Folgende sonstige Personen duerfen "+name(WEN,1)+
      " mitbenutzen:");
    str += CountUp(strs);
  }
  else {
    str += BS("Es gibt keine sonstigen Personen, die "+name(WEN,1)+
      " mitbenutzen duerfen.");
  }
  str +=
    "==================================================================="
    "==========\n";

  write(str);

  return 1;
}

// Haken beschriften.
static int cmd_beschriften(string str) {
  string nfm = "Syntax: sbeschrifte haken <hakennummer> mit "
    "<text>|nichts\n"
    "Fuer <text> kannst Du einen beliebigen Text mit bis zu 30 Zeichen "
    "angeben,\n"
    "'nichts' loescht die Beschriftung eines Schildes.\n";

  notify_fail("Das darfst Du nicht!\n");
  if(!erlaubt()) 
    return 0;
  
  string text;
  int hnr;
  str = TP->_unparsed_args(0);
  notify_fail(nfm);
  if ( !stringp(str) || !sizeof(str) ) 
    return 0;
  if ( sscanf(str,"haken %d mit %s", hnr, text)!=2 )
    return 0;

  notify_fail("Welchen Haken moechtest Du beschriften?\n"+nfm);
  if(!hnr) 
    return 0;
  
  notify_fail("Es gibt nur die Haken 1 bis 16.\n");
  if ( hnr < 1 || hnr > 16 ) 
    return 0;

  notify_fail("Womit moechtest Du den Haken beschriften?\n"+nfm);
  if( !stringp(text) || !sizeof(text) )
    return 0;

  notify_fail("Der Text sollte nicht laenger als 30 Zeichen sein.\n");
  if(sizeof(text) > 30) 
    return 0;

  if ( lower_case(text) == "nichts" ) {
    if ( stringp(haken[hnr,HOOK_LABEL]) ) {
      tell_object(TP, BS("Sorgfaeltig wischst Du die Beschriftung des "
        "Hakens "+hnr+" weg."));
      haken[hnr,HOOK_LABEL]=0;
    }
    else {
      tell_object(TP, BS("Du musst Dich geirrt haben. Haken "+hnr+" ist "
        "gar nicht beschriftet."));
    }
  }
  else if ( lower_case(text) == "schluessel" ) {
    tell_object(TP, BS(
      "Du solltest Dir etwas Aussagekraeftigeres einfallen lassen."));
  }
  else {
    tell_object(TP, BS("Du beschriftest das Schildchen ueber Haken "+hnr+
      " mit "+text+"."));
    tell_room(environment(), BS(
      TP->Name(WER)+" schreibt etwas ans Schluesselbrett."), ({TP}));
    haken[hnr,HOOK_LABEL] = text;
  }
  return 1;
}

/***** HILFSFUNKTIONEN *****/

// Beschreibung eines Hakens erzeugen. Wenn <keynum> angegeben wird, 
// wird nur der gesuchte Haken angezeigt, egal ob leer oder nicht. 
// Ansonsten werden die belegten Haken angezeigt, ausser wenn 
// <hooklist_long> gesetzt ist. Dann werden alle Haken aufgelistet.
private varargs string KeyDescription(int keynum) {
  string desc="";
  mapping keylist=([:2]);

  // Liste derHaken auf den/die gesuchten einschraenken.
  // Nur ein Haken soll angeschaut werden. Dies hat immer Prioritaet,
  // d.h. auch bei <sliste alle> wird nur ein Haken gelistet.
  if ( keynum ) {
    m_add(keylist, keynum, m_entry(haken, keynum)...);
  }
  // Wenn <sliste alle> gesetzt ist, alle ausgeben.
  else if ( hooklist_long ) {
    keylist = haken;
  }
  // Ansonster nur die belegten.
  else {
    keylist = filter(haken, 
      function int (int num, <string|object>* values) {
        return objectp(values[0]);
      });
  }

  // Ueber das Mapping mit den selektierten Haken laufen und Ausgaben
  // erzeugen.
  foreach(int num : 1..16) {
    // Hier wird der aktuelle Wert uebersprungen, wenn er nicht enthalten 
    // ist, da die Zuweisung in <key> und <keydesc> auch dann eine 0 ergeben
    // wuerde, und somit Keys mit leeren Values nicht von nicht existierenden
    // Keys zu unterscheiden waeren.
    if ( member(m_indices(keylist), num) == -1 )
      continue;
    string keydesc = keylist[num, HOOK_LABEL];
    object key = keylist[num, HOOK_OB];
    desc += "Haken " + num + (stringp(keydesc)?" ["+keydesc+"]: " : ": ");
    desc += (objectp(key) ? key->short() : "leer\n");
  }
  return desc;
}

// Rueckgabewert: 1, wenn es sich um ein Schluesselobjekt handelt, sonst 0.
// Schluessel werden anhand der ID "schluessel" erkannt oder daran, dass
// QueryDoorKey() einen Wert zurueckgibt, d.h. dass dieses Objekt eine
// Standard-Doormaster-Tuer oeffnet.
private int IsValidKey(object ob) {
  return (objectp(ob) && (ob->id("schluessel") || ob->QueryDoorKey()!=0));
}

// Gibt die Nummer des naechsten freien Hakens zurueck oder 0, wenn
// keiner frei ist. Da Mappings nicht sortiert sind, muss von 1-16 numerisch
// iteriert werden, damit immer der erste freie Haken gefunden wird und nicht 
// irgendeiner.
// Rueckgabewert: Hakennummer von 1-16 oder 0, wenn kein Haken frei ist.
private int GetNextFreeSlot() {
  int slot;
  foreach(int i : 1..16) {
    if ( !objectp(haken[i,HOOK_OB]) ) {
      slot = i;
      break;
    }
  }
  return slot;
}

// Gibt zu einer Schild-Beschriftung die zugehoerige Hakenposition aus.
// Es wird die kleingeschriebene Variante gesucht, so dass Spieler zwar
// Grossschreibung in den Beschriftungen verwenden koennen, diese aber
// nicht beruecksichtigt wird. Ansonsten koennte es zu Schwierigkeiten mit
// Dopplungen wie "Vesray" und "vesray" kommen.
// Rueckgabewert: Hakennummer von 1-16 oder 0, wenn der Text nicht gefunden
// wurde.
private object GetKeyForLabel(string label) {
  if ( label ) 
    label = lower_case(label);
  foreach(int i: 1..16) {
    if ( haken[i, HOOK_LABEL] && label &&
         lower_case(haken[i,HOOK_LABEL]) == label )
      return haken[i,HOOK_OB];
  }
  return 0;
}

// Gibt zu dem uebergebenen Objekt die Nummer des Hakens zurueck, an dem
// es haengt.
private int GetHookNumberForKey(object ob) {
  foreach(int num, object key : haken) {
    if ( key == ob )
      return num;
  }
  return 0;
}

// Bereinigt Datenstruktur in <haken> und das Inventar des Brettes, indem
// a) geprueft wird, ob alle in <haken> eingetragenen Objekte sich auch im 
//    Brett befinden und
// b) geprueft wird, ob alle im Inventar befindlichen Objekte auch korrekt in 
//    <haken> eingetragen sind.
// Ueberzaehlige Objekte werden rausgeworfen.
private void ConsolidateInventory() {
  foreach(int i, object ob : haken) {
    // Objekt existiert, ist aber nicht im Schluesselbrett? Dann austragen.
    if (objectp(ob) && environment(ob) != ME) {
      m_delete(haken, i);
      // Wird auch vom reset() aus gerufen, dann ist TP == 0.
      if ( objectp(TP) )
        tell_object(TP, BS("Hoppla! "+ob->Name(WER,1)+" scheint vom "
          "Schluesselbrett verschwunden zu sein!"));
    }
  }
  object *resterampe = all_inventory(ME) - m_values(haken, HOOK_OB);
  if (sizeof(resterampe)) {
    resterampe->move(environment(),M_PUT);
    tell_room(environment(), BS("Ploetzlich faellt etwas vom "
      "Schluesselbrett zu Boden, das Du zuvor gar nicht gesehen hattest."));
  }
}

string short() {
  string sh=QueryProp(P_SHORT);

  // Unsichtbar? Dann gibts nichts zu sehen ...
  if ( QueryProp(P_INVIS) || !sh )
    return 0;
  if ( QueryProp("versteckt") == 1 ) 
    sh = "";
  else 
    sh += ".";

  return process_string(sh)+"\n";
}

// Im Reset vorsichtshalber auch mal aufraeumen.
void reset() {
  ConsolidateInventory();
  return ::reset();
}

varargs int PreventInsert(object ob) {
  // Kein Schluessel? Hat in diesem Container nix verloren!
  if( !IsValidKey(ob )) {
    return 1;
  }
  ConsolidateInventory();
  return ::PreventInsert(ob);
}

// Wenn ein Schluessel sich aufloest, muss die Liste abgeglichen werden.
void NotifyRemove(object ob) {
  tell_room(environment(ME), BS("Hoppla! "+ob->Name(WER,1)+" hat sich "
    "gerade in Wohlgefallen aufgeloest."));
  // Das Objekt <ob> zerstoert sich erst nach dem Aufruf dieser Funktion,
  // daher per call_out() aufraeumen.
  call_out(#'ConsolidateInventory, 1);
}

// Zesstra, 1.7.07, fuers Hoerrohr
// Darf ich da Arathorn eintragen? ;-)
string GetOwner() {
  return "seleven";
}

