//--------------------------------------------------------------------------------
// Name des Objects:    Schrankmaster
// Letzte Aenderung:    19.03.2001
// Magier:              wurzel
//--------------------------------------------------------------------------------
/* Changelog:
   * 21.06.2007, Zesstra
           call_out ohne Verzoegerung beim Updaten durch eines mit 2s Verzoegerung ersetzt.
*/
#pragma strong_types,rtt_checks

inherit "/std/room";

#include <properties.h>
#include <language.h>
#include "schrankladen.h"

mapping schraenke;
string *cnts;
int update_laeuft;
int reset_laeuft;

protected void create() 
{
  ::create();
  SP(P_INT_SHORT, "Irgendwo und Nirgends");
  SP(P_INT_LONG,
     "Dies ist der Master-Raum fuer den SCHRAENKER.\n"
    +"Hier merkt sich August die Anzahl der verkauften Container.\n"
    +"Davon sind momentan @@ANZAHL@@ registrierte im Umlauf!\n"
    +"Folgende Befehle sind verfuegbar:\n"
    +"  schrankliste.........Listet registrierte, im Spiel befindliche Container.\n"
    +"  schrankupdate........Fuehrt ein Update aller registrierten Container durch.\n");

  SP(P_LIGHT, 1);
  SP(P_INDOORS, 1);
  if( SCHRANKMASTER2->QueryProp("schraenke") )  // Der Sicherungsraum hat Daten?
    schraenke=SCHRANKMASTER2->QueryProp("schraenke");  // Daten holen...
  else
    schraenke=([]);
  SetProp("schraenke", schraenke );

  update_laeuft=0;

  // unnoetig, SCHRANKMASTER2 ist jetzt schon geladen, s.o.
  //load_object(SCHRANKMASTER2);
  set_next_reset(3600); // Reset 1 x pro Std.

  AddCmd(({"schrankliste"}), "Schrankliste");
  AddCmd(({"schrankupdate"}), "SchrankUpdate");
}

public int clean_up(int ref) {return 0;}   // verhindert Ausswappen des Raumes

public varargs string GetExits( object viewer ) 
{
  return "";
}

string ANZAHL()
{
  return to_string( sizeof(schraenke) );
}


// cnt = Object des Containers
// ver = Version des Containers (VERSION_STD+VERSION_OBJ von std.cont. + objekt...)
// seher=Der Besitzer/Kaeufer
// standort=Raum in dem der Container steht
void RegisterCnt(mixed cnt, mixed ver, mixed seher, mixed standort)
{
  if (objectp(standort)) standort=object_name(standort);
  if( !objectp(cnt) || !stringp(ver) || !stringp(seher) || !stringp(standort) )
  {
    log_file("swift/schraenker/bugs.log", "schrankmaster (RegisterCnt) *"
      +to_string(cnt)+"*"+to_string(ver)+"*"+to_string(seher)+"*"
      +to_string(standort)+"*\n");
    return;
  }
  m_delete(schraenke,object_name(cnt));
  schraenke+=([object_name(cnt): ver; seher; to_string(standort)]);
  SetProp("schraenke", schraenke );
  SCHRANKMASTER2->SetProp("schraenke", schraenke );
}

void RemoveCnt(mixed cnt, mixed seher)
{
  if( !objectp(cnt) || !stringp(seher) )
  {
    log_file("swift/schraenker/bugs.log", "schrankmaster (RemoveCnt) *"
      +to_string(cnt)+"*"+to_string(seher)+"*\n");
    return;
  }
  schraenke-=([object_name(cnt)]);
  SetProp("schraenke", schraenke );
  SCHRANKMASTER2->SetProp("schraenke", schraenke );
}

string fn(string was)
{
  string str,*strs;
  strs=old_explode( was, "/" );
  str=strs[sizeof(strs)-1];
  strs=old_explode( str, "#" );
  str=strs[0];
  return str;
}

int Schrankliste()
{
  int i;
  string str, *strs;
  object cnt;
  if(update_laeuft && !reset_laeuft) // Nicht waehrend eines Updates der Schraenke!
  {
    write("Update wird durchgefuehrt. Bitte bis zum Ende warten!\n");
    return 1;
  }
  str="----------------------------------------------------------------------\n";
  strs=m_indices(schraenke);
  for(i=0;i<sizeof(strs);i++)
  {
    if( cnt=find_object(strs[i]) )
    {
      str+=(fn(object_name(cnt))+"....................")[0..20]+" ";
      str+=schraenke[strs[i], 0]+" ";
      str+=(schraenke[strs[i], 1]+"....................")[0..20]+" ";
      str+=fn(schraenke[strs[i], 2])+"\n";
    }
    else
    {
      str+="Nicht auffindbaren Container aus Liste geloescht!\n";
      schraenke-=([strs[i]]);
      SetProp("schraenke", schraenke );
      SCHRANKMASTER2->SetProp("schraenke", schraenke );
    }
  }
  str+="----------------------------------------------------------------------\n";
  if( TP && present(TP) ) //wurzel 19.03.01 
    TP->More(str);
  return 1;
}

void Update_naechster_Schrank()
{
  int i;
  object cnt, cnt_neu, *inv;
  if( sizeof(cnts) )    // Es gibt noch Schraenke zum abarbeiten...
  {
    cnt=find_object(cnts[0]); // Schrank auswaehlen + Objekt suchen
    if(cnt)
    {
      // Autoloadertruhe ist etwas spezieller zu updaten... Daher alternativer
      // Mechanismus: UpdateMe() im Moebelstueck aufrufen. Wenn die Funktion
      // etwas != 0 liefert, wird angenommen, dass diese Funktion das Update
      // gemacht hat oder sich in Kuerze darum kuemmert. Sonst wird das alte
      // Update gemacht.
      // Das neue Moebelobjekt ist dafuer verantwortlich, sich beim
      // Schrankmaster zu (de-)registrieren!
      object oldenv=environment(cnt);
      if (cnt->UpdateMe())
      {
          tell_room(TO, "...Update "+CAP(fn(cnts[0]))
              +" in " + object_name(oldenv) + "\n" );
      }
      else
      {
          tell_room(TO, CAP(fn(cnts[0]))
              +" in " + object_name(oldenv) 
              + " hat kein UpdateMe()!\n" );
      }
    }
    else
    {
      tell_room(TO, "Nicht auffindbaren Container aus Liste geloescht!\n");
      schraenke-=([cnts[0]]);
    }
    cnts-=({cnts[0]});   // Schrank aus Liste nehmen
    call_out("Update_naechster_Schrank", 2);  // Nur 1 Schrank-Update pro heart_beat!
  }
  else  // Alle Schraenke abgearbeitet:
  {
    tell_room(TO, "*** Update abgeschlossen.\n");
    SetProp("schraenke", schraenke );
    SCHRANKMASTER2->SetProp("schraenke", schraenke );
    update_laeuft=0;
  }
}

int SchrankUpdate()
{
  if(update_laeuft)   // Nicht waehrend eines Updates der Schraenke!
  {
    write("Update wird durchgefuehrt. Bitte bis zum Ende warten!\n");
    return 1;
  }
  update_laeuft=1;    // Ab sofort laeuft Update!
  cnts=m_indices(schraenke);
  tell_room(TO, "*** Update wird gestartet...\n");
  Update_naechster_Schrank();
  return 1;
}


void reset()
{
  reset_laeuft=1;
  set_next_reset(3600); // Reset 1 x pro Std.
  ::reset();
  // Wirft Schraenke aus der Liste, die aus welchem Grund auch immer nicht mehr existieren
  Schrankliste();
  if( !(schraenke==QueryProp("schraenke")==SCHRANKMASTER2->QueryProp("schraenke")) )
    log_file("swift/schraenker/reset.log", "schrankmaster (reset) "
      +sizeof(schraenke)+" "
      +sizeof(QueryProp("schraenke"))+" "
      +sizeof(SCHRANKMASTER2->QueryProp("schraenke"))+"\n");
  if( present("Interactive") )
    tell_room(TO, "*** RESET ***\n");
  reset_laeuft=0;
}

/*
// Veraltete Funktionen, um Fehler abzufangen. Kann nach dem naechsten Reboot
// geloescht werden !!!
void AddCnt(mixed cnt, mixed ver, mixed seher, mixed standort)
{
  log_file("swift/schraenker/verloren.log", to_string(cnt)+"\n");
}
void EditCnt(mixed cnt, mixed ver, mixed seher, mixed standort)
{
  log_file("swift/schraenker/verloren.log", to_string(cnt)+"\n");
}
*/
