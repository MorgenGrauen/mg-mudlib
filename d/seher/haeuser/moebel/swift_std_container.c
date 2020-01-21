//----------------------------------------------------------------------------
// Name des Objects:    Standard_Container
// Letzte Aenderung:    21.03.2001
// Magier:              Swift
// Dieses File wird anstelle von /std/container inherited, so dass dem Objekt
// erweiterte Moeglichkeiten zur Verfuegung stehen.
//
// 07.12.2005 (Seleven): Kurzbeschreibung ausblendbar gemacht
// 30.06.2007 (Zesstra): create der BP wird nicht mehr abgebrochen. Auf die
//   Art wird es dem Nutzer dieses Standardobjekt ueberlassen, ob er die BP
//   initialisieren will oder nicht.
// 01.07.2007 (Zesstra): Update der Moebel ruft erstmal UpdateMe() in den
//   Moebeln auf. Wenn das != 0 liefert, ist UpdateMe() fuer das Update
//   zustaendig und muss das machen.
//   Ausserdem kleine Aenderung im Registrieren (nimmt auch Obekte als
//   Standort)
//   Weiterhin Moebel umgezogen nach /p/seher/moebel/ und in diesem File
//   GetOwner() definiert, welches "Swift" zurueckgibt. Wer hiervon erbt,
//   sollte damit also sein eigenes GetOwner() definieren.
//   glob. Variablen auf 'nosave' geaendert.
//----------------------------------------------------------------------------


// Fuer Testzwecke ist es moeglich, dem entsprechenden Objekt die Property
// "test" auf 1 zu setzen, damit ist dann so ziemlich alles erlaubt.
// Man sollte die Property aber unbedingt wieder entfernen, sonst ist es z.B.
// moeglich, einen Schrank von Seherhaus A nach Seherhaus B oder sogar hinaus
// zu verschieben !!!

#pragma strong_types,save_types,rtt_checks

#include <container.h>
inherit "/std/container";

#include "schrankladen.h"

#define VERSION_STD "10"

nosave int hauserlaubnis;
nosave string familie;
nosave string *erlaubnis = ({});

int erlaubt();

protected void create()
{
  //if (!clonep(TO)) return;
  ::create();
  SetProp(P_SHORT, "Ein Standard_Container von Swift");
  SetProp(P_CNT_STATUS,CNT_STATUS_CLOSED);
  SetProp(P_TRANSPARENT, 0);
  SetProp(P_LOG_FILE,"seleven/schraenker"); // Seleven 06.04.2006
  //TODO: Die ganzen cnt_* Props sollten besser eindeutige Praefixe sein,
  //nicht das generische cnt_, was auch die Mudlib verwendet. :-( (Zesstra)
  SetProp("cnt_version_std", VERSION_STD);

  set_next_reset(3600); // 1 x pro Std. Reset!

  AddCmd(({"oeffne"}), "oeffnen");
  AddCmd(({"schliess", "schliesse"}), "schliessen");
  AddCmd(({"serlaube"}), "erlaubnis");
  AddCmd(({"skommandos"}), "skommandos");
  AddCmd(({"verschieb", "verschiebe"}), "verschieben");
  AddCmd(({"zertruemmer", "zertruemmere"}), "zertruemmern");
  AddCmd(({"sverstecke","sversteck"}),"verstecken");
  AddCmd(({"shole","shol"}),"zeigen");
}

// Zesstra, 1.7.07, fuers Hoerrohr
string GetOwner() {return "swift";}

static string QueryHausbesitzer()
{
  return CAP(to_string(ETO->QueryOwner()));    // z.B.: Swift
}

static string QueryTP()
{
  return CAP(geteuid(TP));  // z.B.: Swift
}

// Query- und Setmethoden fuer die internen Variablen
static string _query_cnt_familie()
{
  return familie;
}
static string _set_cnt_familie(string f)
{
  familie = f;
  return familie;
}

static string *_query_cnt_erlaubnis()
{
  return erlaubnis;
}
static string *_set_cnt_erlaubnis(string *val)
{
  erlaubnis = val;
  return erlaubnis;
}

static int _query_cnt_hausfreunde()
{
  return hauserlaubnis;
}
static int _set_cnt_hausfreunde(int val)
{
  hauserlaubnis = val;
  return hauserlaubnis;
}

int oeffnen(string str)
{
  notify_fail("Was moechtest Du oeffnen?\n");
  if(!str) return 0;
  if(present(str)!=TO)  // Ueberpruefe, ob auch dieses Objekt gemeint ist!
      return 0;
  if(QueryProp(P_CNT_STATUS)==CNT_STATUS_OPEN)
  {
    write(Name(WER,1)+" ist doch schon geoeffnet!\n");
    return 1;
  }
  if( !erlaubt() )
  {
    write("Ein magisches Kraftfeld verhindert das Oeffnen "+name(WESSEN,1)+"!\n");
    return 1;
  }
  write("Du oeffnest "+name(WEN)+".\n");
  say(TP->Name(WER)+" oeffnet "+name(WEN)+".\n", ({TP}) );
  SetProp(P_CNT_STATUS,CNT_STATUS_OPEN);
  SetProp(P_TRANSPARENT, 1);
  return 1;
}

int schliessen(string str)
{
  notify_fail("Was moechtest Du schliessen?\n");
  if(!str) return 0;
  if(present(str)!=TO)   // Ueberpruefe, ob auch dieses Objekt gemeint ist!
    return 0;
  if(QueryProp(P_CNT_STATUS)==CNT_STATUS_CLOSED)
  {
    write(Name(WER,1)+" ist doch schon geschlossen!\n");
    return 1;
  }
  if( !erlaubt() )
  {
    write("Ein magisches Kraftfeld verhindert das Schliessen "+name(WESSEN,1)+"!\n");
    return 1;
  }
  write("Du schliesst "+name(WEN)+".\n");
  say(TP->Name(WER)+" schliesst "+name(WEN)+".\n", ({TP}) );
  SetProp(P_CNT_STATUS,CNT_STATUS_CLOSED);
  SetProp(P_TRANSPARENT, 0);
  return 1;
}

string cnt_status()
{
  return Name(WER,1)+" ist "
    +(QueryProp(P_CNT_STATUS)?"geschlossen":"geoeffnet")+".\n";
}

int erlaubt()
{
  if( QueryProp("test") ) return 1;       // Zu Testzwecken!
  if( QueryHausbesitzer()=="0" )          // Schrank steht nicht in einem Seherhaus!
  {
    tell_room(ETO, Name(WER,1)+" sollte nicht ausserhalb eines Seherhauses stehen.\n"
      +"Bitte Mail an "+VERANTWORTLICHER_MAGIER+"!\n");
    return 0;
  }
  if( QueryTP() == QueryHausbesitzer() )  // Der Besitzer des Hauses selbst
    return 1;
  if(hauserlaubnis)
  {
    if( member(SHVERWALTER->HausProp(LOWER(QueryHausbesitzer()), 2),
        QueryTP()) != -1 )                // Hausfreunde
      return 1;
  }
  if(familie)                // Zweities
  {
    if( "/secure/zweities"->QueryFamilie(TP) == familie )
      return 1;
  }
  if( sizeof(erlaubnis) )
  {
    if( member(erlaubnis, QueryTP()) != -1 ) // Sonstige Personen die berechtigt sind
      return 1;
  }
  return 0;
}

void cnt_open()
{
  SetProp(P_CNT_STATUS,CNT_STATUS_OPEN);
  SetProp(P_TRANSPARENT, 1);
}

int erlaubnis_liste()
{
  int i;
  string *strs,str;
  if( QueryHausbesitzer() != QueryTP() && !QueryProp("test") )
  {
    write( BS("Nur "+QueryHausbesitzer()+" darf Berechtigungen "
      +name(WESSEN,1)+" abfragen!"));
    return 1;
  }
  write("=============================================================================\n");
  if(hauserlaubnis)
  {
    strs=SHVERWALTER->HausProp(LOWER(QueryHausbesitzer()), 2);
    write(BS("Folgende Freunde Deines Hauses duerfen "+name(WEN,1)
      +" oeffnen/schliessen:"));
    str="";
    if(sizeof(strs))
    {
      for(i=0;i<sizeof(strs);i++)
      {
        str+=strs[i];
        if(i<sizeof(strs)-1)
          str+=", ";
      }
    }
    else
      str="Du hast keiner Person Zugriff auf Dein Haus gewaehrt...\n";
    write(BS(str));
  }
  else
    write(BS("Die Freunde Deines Hauses duerfen "+name(WEN,1)
      +" nicht oeffnen/schliessen."));
  write("-------------------------------------------------------------------"
        "----------\n");
  if(familie)
  {
  if( "/secure/zweities"->QueryFamilie(TP) == familie)
    write(BS("Alle Deine Familienmitglieder duerfen "
             +name(WEN,1)+" oeffnen/schliessen."));
  else
    write(BS( "Alle Familienmitglieder von "+CAP(explode(familie,"_")[0])
             +" duerfen " + name(WEN,1) + " oeffnen/schliessen."));
  write("-------------------------------------------------------------------"
        "----------\n");
  }
  strs=QueryProp("cnt_erlaubnis");
  if(sizeof(strs))
  {
    write(BS("Folgende sonstige Personen duerfen "+name(WEN,1)
      +" oeffnen/schliessen:"));
    str="";
    for(i=0;i<sizeof(strs);i++)
    {
      str+=strs[i];
      if(i<sizeof(strs)-1)
        str+=", ";
    }
    write(BS(str));
  }
  else
    write(BS("Es gibt keine sonstigen Personen, die "+name(WEN,1)
      +" oeffnen/schliessen duerfen."));
  write("=============================================================================\n");
  return 1;
}

int erlaubnis(string str)
{
  string *strs,wen,nf_str;
  nf_str="Syntax: serlaube [Objekt-Id] [Spielername|\"hausfreunde\"|\"familie\"]\n"
        +"Bsp.:   serlaube "+QueryProp(P_IDS)[1]+" hausfreunde\n"
        +"        serlaube "+QueryProp(P_IDS)[1]+" zweities\n"
        +"        serlaube "+QueryProp(P_IDS)[1]+" geordi\n"
        +"        serlaube "+QueryProp(P_IDS)[1]+"      (Listet Spieler mit Erlaubnis)\n";
  notify_fail("Fehler: Ohne Parameter klappt das nicht.\n"+nf_str);
  if(!str) return 0;
  if(present(str)==TO)     // Falls Par==Obj. und kein 2./3. Par.
  {
    erlaubnis_liste();           //   Liste der Erlaubten ausgeben!
    return 1;
  }
  notify_fail("Fehler: Du musst eine gueltige Objekt-Id angeben!\n"+nf_str);
  strs=old_explode(str, " ");
  if( sizeof(strs) < 2 )           // Falls nur 1 Par.
    return 0;             // Anzahl erforderlicher Parameter unterschritten!
  else if( sizeof(strs) == 2 )
  {
    str=strs[0];
    wen=strs[1];
    if( IST_ZAHL(wen) )
      if(present(str+" "+wen)==TO)   // Falls 2 Par. und dies das Obj. ist ohne 3. Par.
      {
        erlaubnis_liste();           //   Liste der Erlaubten ausgeben!
        return 1;
      }
      else
        return 0;
  }
  else if( sizeof(strs) == 3 )
  {
    str=strs[0]+" "+strs[1];
    wen=strs[2];
  }
  else        // Anzahl erforderlicher Parameter ueberschritten!
    return 0;
  if(present(str)!=TO)   // Ueberpruefe, ob auch dieses Objekt gemeint ist!
    return 0;
  if( !erlaubt() && !QueryProp("test") )
  {
    write( BS("Nur "+QueryHausbesitzer()+" darf Berechtigungen "
      +name(WESSEN,1)+" aendern!"));
    return 1;
  }
  if(wen=="hausfreunde")
  {
    if(hauserlaubnis)
    {
      hauserlaubnis=0;
      write( BS("Die Freunde Deines Seherhauses duerfen "+name(WEN,1)
                +" jetzt nicht mehr oeffnen/schliessen."));
    }
    else
    {
      hauserlaubnis=1;
      write( BS("Die Freunde Deines Seherhauses duerfen "+name(WEN,1)
                +" jetzt oeffnen/schliessen."));
    }
    return 1;
  }
  if(wen=="familie")
  {
    if(familie)                        // Zweities erlaubt?
    {
      familie = 0;                     //   dann verbieten!
      write("Die Familienerlaubnis wurde geloescht.\n");
    }
    else                               // ansonsten neu erlauben
    {
      familie = "/secure/zweities"->QueryFamilie(TP);
      write(BS("Alle Mitglieder der Familie "
               +CAP(explode(familie,"_")[0])
               +" duerfen "+name(WEN,1)+" jetzt oeffnen/schliessen."));

    }
    return 1;
  }
  if( master()->find_userinfo(wen)) // Spieler gibt es auch!
  {
    wen=CAP(wen);
    if( member(erlaubnis, wen) != -1 )  // Spieler hat Erlaubnis -> verbieten!
    {
      erlaubnis-=({wen});
      write( BS( wen+" darf "+name(WEN,1)+" jetzt nicht mehr oeffnen/schliessen."));
    }
    else
    {
      erlaubnis+=({wen});
      write( BS( wen+" darf "+name(WEN,1)+" jetzt oeffnen/schliessen."));
    }
    SetProp("cnt_erlaubnis", erlaubnis);
    return 1;
  }
  else
    write("Es gibt keinen Spieler namens "+CAP(wen)+"!\n");
  return 1;
}

varargs int skommandos(string str)
{
  notify_fail( "Fehler: Dieser Befehl benoetigt eine gueltige Objekt-Id als Parameter.\n"
    +"Beispiel: skommandos "+QueryProp(P_IDS)[1]+"\n");
  if(!str) return 0;
  if(present(str)!=TO ) // Bin ich gemeint?
    return 0;
  write("=============================================================================\n");
  write("Aktuelle Version: "+QueryProp("cnt_version_std")+QueryProp("cnt_version_obj")+"\n");
  write( BS(Name(WER,1)+" kann nur in diesem Seherhaus verwendet werden, "
    +"da "+QueryPronoun(WER)+" speziell dafuer gekauft wurde. "+CAP(QueryPronoun(WER))
    +" verfuegt ueber folgende Kommandos:")
    +"-----------------------------------------------------------------------------\n"
    +"oeffne [Objekt-Id]\n"
    +"  oeffnet "+name(WEN,1)+"\n\n"
    +"schliesse [Objekt-Id]\n"
    +"  schliesst "+name(WEN,1)+"\n\n"
    +"serlaube [Objekt-Id] [Spielername|\"hausfreunde\"|\"familie\"]\n"
    +"  Erlaubt Personen, "+name(WEN,1)+" mitzubenutzen.\n"
    +"  serlaube + Objekt-Id (ohne Spielername/hausfreunde)\n"
    +"  listet alle Personen mit Zugriff auf "+name(WEN,1)+"\n\n"
    +"verschiebe [Objekt-Id] nach [Ausgang]\n"
    +"  Damit kannst Du "+name(WEN,1)+" innerhalb Deines Seherhauses verschieben.\n\n"
    +"sverstecke [Objekt-Id]\n"
    +"  Damit machst Du "+name(WEN,1)+" unsichtbar.\n"
    +"shole [Objekt-Id] hervor\n"
    +"  Damit machst Du "+name(WEN,1)+" wieder sichtbar.\n"
    +"zertruemmer [Objekt-Id]\n"
    +"  Damit zerstoerst Du "+name(WEN,1)+".\n\n");


  if( QueryProp("obj_cmd") )
    write( QueryProp("obj_cmd")+"\n" );

  write("[Objekt-Id] muss eine gueltige Id sein, in diesem Fall z.B. "
      +QueryProp(P_IDS)[1]+"\n");
  write("=============================================================================\n");


  return 1;
}

int verschieben(string str)
{
  string was, wohin, zielraum,nf_str;
  nf_str="Syntax: verschiebe [Objekt-Id] nach [Richtung]\n"
        +"Bsp.:   verschiebe "+QueryProp(P_IDS)[1]+" nach osten\n";
  notify_fail("Fehler: Ohne Parameter klappt das nicht.\n"+nf_str);
  if(!str) return 0;
  if(sscanf(str,"%s nach %s", was, wohin) != 2)
    return 0;
  notify_fail("Fehler: Du musst eine gueltige Objekt-Id angeben!\n"+nf_str);
  if(present(was)!=TO)   // Ueberpruefe, ob auch dieses Objekt gemeint ist!
    return 0;
  if( QueryHausbesitzer() != QueryTP() && !QueryProp("test") )
  {
    write( BS("Nur "+QueryHausbesitzer()+" darf "+name(WEN,1)+" verschieben!"));
    return 1;
  }
  if( member(m_indices(ETO->QueryProp(P_EXITS)), wohin) != -1 ) // Ausgang vorhanden?
  {
    zielraum=ETO->QueryProp(P_EXITS)[wohin];
    if( strstr(zielraum, LOWER(QueryHausbesitzer())+"raum", 0) != -1  ||
        QueryProp("test") )  // Gleiches Seherhaus? Oder Test?
    {
      TO->move(zielraum, M_PUT);
      write("Du verschiebst "+name(WEN,1)+" nach "+CAP(wohin)+".\n");
      TP->move(zielraum, M_GO, "nach "+CAP(wohin), "schiebt "+name(WEN),
                              "schiebt "+name(WEN)+" herein");
      if( present("Interactive", SCHRANKMASTER) )
        tell_room(SCHRANKMASTER, TP->Name(WER)+" verschiebt "+name(WEN)+" nach "
          +to_string(ETO)+".\n");
    }
    else
      write( BS("Du kannst "+name(WEN,1)+" nicht in einen Raum ausserhalb "
        +"Deines Hauses schieben!"));
  }
  else
    write( BS("Es gibt keinen Ausgang namens \""+wohin+"\", wohin Du "
      +name(WEN,1)+" verschieben koenntest.",78,"Fehler: ", BS_INDENT_ONCE));
  return 1;
}

int zertruemmern(string str)
{
  string nf_str;
  nf_str="Syntax: zertruemmer [Objekt-Id]\n"
        +"Bsp.:   zertruemmer "+QueryProp(P_IDS)[1]+"\n";
  notify_fail("Fehler: Ohne Parameter klappt das nicht.\n"+nf_str);
  if(!str) return 0;
  notify_fail("Fehler: Du musst eine gueltige Objekt-Id angeben!\n"+nf_str);
  if(present(str)!=TO)   // Ueberpruefe, ob auch dieses Objekt gemeint ist!
    return 0;
  if( QueryHausbesitzer() != QueryTP() && !QueryProp("test") )
  {
    write( BS("Nur "+QueryHausbesitzer()+" darf "+name(WEN,1)+" zertruemmern!"));
    return 1;
  }
  write( BS("Du zertruemmerst "+name(WEN)+" mit wuchtigen Schlaegen, bis "
    +"nicht mal mehr Kruemel von "+QueryPronoun(WEM)+" uebrig sind."));
  tell_room(ETO, BS( TP->Name(WER)+" zertruemmert "+name(WEN)+"."), ({TP}) );
  call_other(SCHRANKMASTER,"???",0);
  if( present("Interactive", SCHRANKMASTER) )
    tell_room(SCHRANKMASTER, TP->Name(WER)+" zertruemmert "+name(WEN)+".\n");
  SCHRANKMASTER->RemoveCnt(TO, geteuid(TP) );
  remove(1);
  if(TO) destruct(TO);
  return 1;
}

void reset()
{
  set_next_reset(3600); // 1 x pro Std. Reset!
  ::reset();
}

public varargs int remove(int silent)
{
  // Beim Schrankmaster abmelden, dann zerstoeren.
  string uid="";
  if (objectp(environment()))
  {
    uid=environment()->QueryOwner();                
  }
  SCHRANKMASTER->RemoveCnt(TO, uid );
  return ::remove(silent);
}

public int UpdateMe()
{
  if (!objectp(environment()))
    return 0;
  object cnt_neu=clone_object(load_name(ME));  // Neuen Schrank clonen
  cnt_neu->move( environment(), M_NOCHECK );  // In selben Raum wie alten schieben...
  cnt_neu->SetProp(P_CNT_STATUS, QueryProp(P_CNT_STATUS) );
  cnt_neu->SetProp(P_TRANSPARENT, QueryProp(P_TRANSPARENT) );
  cnt_neu->SetProp("cnt_hausfreunde", QueryProp("cnt_hausfreunde") );
  cnt_neu->SetProp("cnt_erlaubnis", QueryProp("cnt_erlaubnis") );
  cnt_neu->SetProp("cnt_familie", QueryProp("cnt_familie") );
  // Inventar bewegen
  foreach(object inv: all_inventory(this_object()))
  {
    if( inv->IsUnit() )                          // Units
      inv->move(cnt_neu, M_NOCHECK|M_MOVE_ALL);
    else                                            // Normale Objekte
      inv->move(cnt_neu, M_NOCHECK);
  }
  remove(1);
  return 1;
}

// Eingebaut 07.12.2005 von Seleven
// Die Kurzbeschreibung invis machen:
static int verstecken (string str)
{
   string nf_str;
   nf_str="Syntax: sverstecke [Objekt-Id]\n"
         +"Bsp.:   sverstecke "+QueryProp(P_IDS)[1]+"\n";
   notify_fail("Fehler: Ohne Parameter klappt das nicht.\n"+nf_str);
   if(!str || sizeof(str) == 0 || !stringp(str)) return 0;

   notify_fail("Fehler: Du musst eine gueltige Objekt-Id angeben!\n"+nf_str);
   if(present(str)!=TO)   // Ueberpruefe, ob auch dieses Objekt gemeint ist!
    return 0;

  if( QueryHausbesitzer() != QueryTP() && !QueryProp("test") )
  {
    write( BS("Nur "+QueryHausbesitzer()+" darf "+name(WEN,1)+" verstecken!"));
    return 1;
  }
  notify_fail("Du hast "+name(WEN,1)+" doch schon versteckt!\n");
  if(QueryProp("versteckt") == 1) return 0;
  write("Du versteckst "+name(WEN,1)+".\n");
  SetProp("versteckt", 1);
  return 1;
}

static int zeigen (string str)
{
   string nf_str;
   nf_str="Syntax: shole [Objekt-Id] hervor\n"
         +"Bsp.:   shole "+QueryProp(P_IDS)[1]+" hervor\n";
   notify_fail("Fehler: Ohne Parameter klappt das nicht.\n"+nf_str);
   if(!str || sizeof(str) == 0 || !stringp(str)) return 0;
   if(strstr(str,"hervor") != -1)
    sscanf(str,"%s hervor",str);
   notify_fail("Fehler: Du musst eine gueltige Objekt-Id angeben!\n"+nf_str);
   if(present(str)!=TO)   // Ueberpruefe, ob auch dieses Objekt gemeint ist!
    return 0;

   if( QueryHausbesitzer() != QueryTP() && !QueryProp("test") )
   {
     write( BS("Nur "+QueryHausbesitzer()+" darf "+name(WEN,1)+
               " wieder hervorholen!"));
     return 1;
   }
   notify_fail(Name(WER,1)+" ist doch gar nicht versteckt!\n");
   if(QueryProp("versteckt") == 0) return 0;
   write("Du holst "+name(WEN,1)+" aus dem Versteck.\n");
   SetProp("versteckt", 0);
   return 1;
}


mixed short()
{
  string sh;

  // Unsichtbar? Dann gibts nichts zu sehen ...
  if (QueryProp(P_INVIS)||!(sh=QueryProp(P_SHORT)))
  return 0;

  if(QueryProp(P_CNT_STATUS)==CNT_STATUS_OPEN)
  {sh += " (geoeffnet)";}

  if(QueryProp("versteckt") == 1) {sh = "";}
  else sh += ".";

  return process_string(sh)+"\n";
}

// hier nicht genutzt, zum Ueberschreiben gedacht.
protected void SetBesitzer(string uid, string uuid) { }

// nach dem move schauen, ob man in einem Seherhaus ist. Wenn ja, wird die
// Funktion SetBesitzer() gerufen und ihr UID und UUID des Eigentuemers
// uebergeben. Sollten UID und/oder UUID nicht bestimmbar sein, wird die
// Funktion nicht gerufen.
protected void NotifyMove(object dest, object oldenv, int method)
{

  ::NotifyMove(dest, oldenv, method);

  if (dest
      && strstr(object_name(dest),"/d/seher/haeuser/") == 0)
  {
    // (vermutlich) in einem Seherhaus
    string uid = dest->QueryOwner();
    // erstmal beim Schrankmaster anmelden.
    SCHRANKMASTER->RegisterCnt(TO, QueryProp("cnt_version_std")
        +":"+QueryProp("cnt_version_obj"), uid, dest);
    // wenn uid ein Spieler ist, dann SetBesitzer rufen.
    if (master()->find_userinfo(uid))
    {
      SetBesitzer(uid, getuuid(uid));
    }
    // else: User unbekannt? -> jedenfalls keine initialisierung moeglich.
  }
}

