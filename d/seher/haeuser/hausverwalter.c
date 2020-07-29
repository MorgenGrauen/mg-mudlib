/*
 *  hausverwalter.c -- Verwaltung der Seherhaeuser
 *
 *  Das Grundobjekt stammt von Boing, Fortfuehrung erfolgte durch Jof.
 *  Letzte Aenderungen verschuldet durch Wargon ;)
 *
 * $Date: 1997/09/09 17:19:29 $
 * $Revision: 2.3 $
 * $Log: hausverwalter.c,v $
 * Revision 2.3  1997/09/09 17:19:29  Wargon
 * Bugfix beim Verlegen/Loeschen eines Hauses
 *
 * Revision 2.2  1996/02/21  18:15:02  Wargon
 * *** empty log message ***
 *
 * Revision 2.0  1994/11/17  13:48:27  Wargon
 * Modifikationen fuer die Trennung Haus/Raum.
 *
 * Revision 1.5  1994/10/24  08:21:55  Wargon
 * Parameter fuer NeuesHaus geaendert.
 * Fuer Sicherheitscheck secure() eingebaut.
 * VerlegeHaus() eingebaut, falls ein Haus mal verlegt werden muss.
 *
 * Revision 1.4  1994/10/10  21:50:59  Wargon
 * NeuesHaus() und LoescheHaus() bedienen nun auch den OBJECTD.
 * PruefeHaus() wurde damit hinfaellig.
 *
 * Revision 1.3  1994/10/09  20:11:48  Wargon
 * Beschreibung der Haeuser vom Hausverwalter abgekoppelt!
 * Die megamap enthaelt nur noch Besitzer und Standort des Hauses.
 * Infolgedessen sind Save() und build() rausgeflogen...
 *
 * Revision 1.2  1994/10/07  22:19:48  Wargon
 * AUFBAU DES MAPPINGS GEAENDERT! Der Filename des Raumes, in dem das
 * Haus steht, steht jetzt als erster Eintrag im Mapping! Alle anderen
 * Eintraege sind um 1 weitergewandert.
 * Beim Laden des Verwalters werden nicht mehr alle Seherhaeuser ge-
 * laden. Ein Haus wird erst geladen, wenn der Raum, in dem es steht,
 * geladen wird (PruefeHaus(), siehe auch std/room::create()).
 *
 * Revision 1.1  1994/10/07  14:19:36  Wargon
 * Initial revision
 *
 */
#pragma strict_types
#include <properties.h>
#include <wizlevels.h>
#include <rooms.h>
#include <moving.h>
#include "haus.h"

#define H_MAX_ROOMS 9

mapping megamap;

// Haus fuer owner im Raum env erstellen. Wird i.d.R nur vom Instanthaus gemacht.
void NeuesHaus(string owner, object env);

// Haus von owner loeschen (samt Savefile!). Dieser Vorgang ist unwiderruflich!
int LoescheHaus(string owner);

// Loescht den letzten hinzugefuegten Raum im Seherhaus von 'owner'.
void LoescheRaum(string owner);

// Fuegt einen Raum zum Seherhaus von 'owner' hinzu.
void NeuerRaum(string owner);

// Haus von owner vom Raum 'von' in den Raum 'nach' verschieben.
int VerlegeHaus(string owner, string von, string nach);

// Kann in ob ein Haus gebaut werden? 0: Ja, sonst Fehler!
int Unbebaubar(object ob);

// Jemandem im Haus Zusatzrechte einraeumen/entziehen
string *Erlaube(string owner, string *wer);
string *Verbiete(string owner, string *wer);

// Eigenschaften aus der megamap abfragen
mixed HausProp(string owner, int prop);

// Propertymapping deduplizieren
mixed PCrunch(mapping prop);

// Lade Seherhaus von Besitzer 'owner'
object _LadeHaus(string owner);

// Lade im Seherhaus von 'owner' den Raum mit der Nummer 'num'
object _LadeRaum(string owner, int num);

// returnt das Seherhaus von Besitzer 'owner'
object FindeHaus(string owner);

void create()
{
  if (!restore_object(SAVEFILE))
    megamap = ([ ]);
  seteuid(getuid(this_object()));
}

int query_prevent_shadow(object ob)
{
  HLOG("SHADOW",sprintf( "%s, von %O im Verwalter.\n",dtime(time())[5..], ob));
  return 1;
}

private int
secure()
{
  int ar;

  if (!this_interactive())
    return 0;

  // das tragbare Instanthaus und die Hausausgabe duerfen:
  if ((load_name(previous_object()) == PATH+"traghaus") ||
      (load_name(previous_object()) == PATH+"sb_ausgabe")) {
    return 1;
  }
  
  catch(ar = ({int})(PATH+"access_rights")->access_rights(
                      geteuid(this_interactive()), "haus.h"));

  // Erzmagier und alle mit Schreibrechten auf haus.h duerfen
  if ( (this_interactive() == this_player()) &&
      (IS_ARCH(this_interactive()) || ar ) )
    return 1;
  return 0;
}

// ersetzt das HAEUSER logfile mit neuer Statistik
private void
dump()
{
  string *ind;
  int i, hnum, rnum = 0;

  // loesche logfile
  rm(PATH+"HAEUSER");

  // betrachte alle Seherhaeuser
  ind = m_indices(megamap);
  if (hnum=sizeof(ind)) {
    write_file(PATH+"HAEUSER", sprintf("Es gibt %d Seherhaeuser:\n", hnum));
    ind = sort_array(ind,#'>); //'
    // alle Haeuser sortiert nach Besitzername durchgehen:
    for(i = 0; i < hnum; ++i) {
      // zaehle Raeume
      ++rnum; // Hauptraum
      rnum += (megamap[ind[i], HP_ROOMS]); // Nebenraeume
      // Eine Zeile pro Haus: Besitzername (Raumanzahl) Standort-Pfad
      write_file(PATH+"HAEUSER",
                 sprintf( "%-13s (%d) %s\n",
                          capitalize(ind[i]),
                          megamap[ind[i],HP_ROOMS],
                          megamap[ind[i],HP_ENV] ) );
    }
    write_file(PATH+"HAEUSER", sprintf("Es gibt insgesamt %d Raeume.\n", rnum));
  }
  else
    write_file(PATH+"HAEUSER", "KEINE HAEUSER!\n");
}

// Gegenrichtungen
#define X_EXIT (["oben":"unten", "unten":"oben",\
		 "westen":"osten", "osten":"westen",\
		 "sueden":"norden", "norden":"sueden",\
		 "nordosten":"suedwesten", "suedwesten":"nordosten",\
		 "nordwesten":"suedosten", "suedosten":"nordwesten" ])

// fuer jeden Raum im Haus [max .. 0] betrachte alle Ausgaenge;
// zaehle alle Ausgaenge ausser der Haustuer in Raeume,
// die nicht zu diesem Seherhaus gehoeren
// (dies sollten Uebergaenge zu anderen Seherhaeusern sein)
// falls rem != 0 loesche die Gegenrichtung zu diesen Ausgaengen,
// d.h. kappe alle Uebergaenge aus anderen Seherhaeusern in dieses
private int
check_exits(string owner, int max, int rem)
{
  int x, nr, bar;
  string hname, foo;
  object here, there;

  x = 0;
  for (nr = max; nr >= 0; --nr) {
    // betrachte jeden Seherhausraum mit index max .. 0
    hname = RAUMNAME(owner, nr);

    if (catch(here = load_object(hname);publish)) {
      printf("error loading %O!\n", hname);
      continue;
    }
    foreach (string dir, string path : ({mapping})(here->QueryProp(P_EXITS)))
    {
      // betrachte alle Ausgaenge
      if (dir == "raus") {
        // Haustuer aus dem Hauptraum darf natuerlich rausfuehren
        continue;
      }
      if ((sscanf(path, PATH+"%sraum%d", foo, bar) != 2) || (foo != owner)) {
        // Raum in den der Ausgang fuehrt ist nicht in diesem Seherhaus
        ++x;
        if (rem) {
          catch(there = load_object(path);publish);
          if (there) {
            // loesche die Gegenrichtung zu dem Ausgang
            there->RemoveExit(X_EXIT[dir]);
            there->Save();
          }
        }
      }
    }
  }
  return x;
}

// Haus fuer owner im Raum env erstellen.
// Wird i.d.R nur vom Instanthaus gemacht.
void NeuesHaus(string owner, object env)
{
  object h;

  // keine passenden Rechte
  if (!secure())
    return;

  // neuen Eintrag im Verwalter-Mapping fuer das Haus erstellen
  megamap += ([ owner : object_name(env); 0; ({}) ]);
  // Haus am Bauplatz laden, falls moeglich
  catch(h = load_object(HAUSNAME(owner));publish);
  if (!h)
    return;

  // Haus Speichern und als Raumautoloader eintragen
  h->Save();
  OBJECTD->AddObject(h, object_name(env));
  // Bauplatz auf never clean setzen und Verwalter abspeichern
  env->SetProp(P_NEVER_CLEAN, 1);
  save_object(SAVEFILE);

  // Hauptraum des Seherhauses laden
  h = load_object(RAUMNAME(owner,0));
  h->SetProp(H_CHEST,1);
  // Hauptraum speichern
  h->Save();
  // Truhe laden
  h->Load();
  // Statistik ueber alle Seherhaeuser erneuern
  dump();
}

// loescht den letzten hinzufuegten Raum im Seherhaus von 'owner'
void LoescheRaum(string owner)
{
  object raumob;
  int nr;

  // kein passendes Seherhaus verwaltet oder kein Recht das zu tun
  if (!member(megamap, owner) || !secure())
    return;

  nr = megamap[owner, HP_ROOMS];
  // falls das Haus ueberhaupt zusaetzliche Raeume (neben Hauptraum) hat
  if (nr > 0 ) {
    // falls geladen, remove Raum-Objekt
    raumob = find_object(RAUMNAME(owner,(megamap[owner,HP_ROOMS])));   
    if (objectp(raumob))
      raumob->remove(1);

    // loesche Raum aus Verwalter-Mapping durch Anzahl um eins erniedrigen
    --megamap[owner, HP_ROOMS];

    // savefile muss per Hand geloescht werden:
    tell_object(this_interactive(),
                break_string(sprintf("Vergiss nicht, das Savefile zu loeschen, "
                                     "also: "+HAUSSAVEPATH+"%s%d.o\n",
                                     owner, nr),
                             78));
    // speicher Hausverwaltung ab und erneuer Statistik ueber alle Seherhaeuser
    save_object(SAVEFILE);
    dump();
  }
}

// Fuegt einen Raum zum Seherhaus von 'owner' hinzu.
void NeuerRaum(string owner)
{
  object raumob;

  // kein passendes Seherhaus verwaltet oder kein Recht das zu tun
  if (!member(megamap, owner) || !secure())
    return;

  // ist die Maximalanzahl von Raeumen schon erreicht?
  if (megamap[owner, HP_ROOMS] < H_MAX_ROOMS)
  {
    // erhoehe Raumzaehler in Verwalter-Mapping
    megamap[owner, HP_ROOMS]++;
    // lade neuen Raum, falls moeglich
    catch(raumob = load_object((RAUMNAME(owner,(megamap[owner,
                                         HP_ROOMS]))));publish);
    if(objectp(raumob))
      // speicher neuen Raum
      raumob->Save();

    // speicher Verwalter-Mapping und erneuer Statistik ueber alle Seherhaeuser
    save_object(SAVEFILE);
    dump();
  }
}

// Lade Seherhaus von Besitzer 'owner'
object _LadeHaus(string owner)
{
  object haus;
  string o;

  // es wird kein passendes Seherhaus verwaltet
  if (!member(megamap, owner))
    return 0;

  // Haus ist bereits geladen
  if (haus=find_object(HAUSNAME(owner)))
    return haus;

  // lade Bauplatz
  o = megamap[owner];
  if (catch(load_object(o);publish))
  {
    write_file(PATH+"hauserror", o+" konnte nicht geladen werden.\n");
    return 0;
  }
  // Haus ist durch Laden des Bauplatzes nun geladen
  if (haus = find_object(HAUSNAME(owner)))
    return haus;

  // clone Standard-Haus, setze Besitzer
  haus = clone_object(HAUS);
  haus->move(o, M_NOCHECK);
  haus->SetOwner(owner, find_object(RAUMNAME(owner,0)));
  // lade individualisiertes Haus aus dem Savefile
  haus->Load();

  return haus;
}

// Lade im Seherhaus von 'owner' den Raum mit der Nummer 'num'
object _LadeRaum(string owner, int num)
{
  object raum;

  // es wird kein passendes Seherhaus verwaltet
  if (!member(megamap, owner))
    return 0;

  // Raumnummer nicht zwischen 0 und letzter Raumnummer
  if (num < 0 || num > megamap[owner,HP_ROOMS])
    return 0;

  // Raum ist bereits geladen
  if (raum = find_object(RAUMNAME(owner,num)))
    return raum;

  // clone passenden Raum (0: Hauptraum, X: Nebenraum X) und setze Besitzer
  raum = clone_object(num ? (RAUM) : (PATH+"raum0"));
  raum->SetOwner(owner, num);
  // lade Moebel, z.B. Seherhaustruhe
  raum->Load();
  // Hauptraum bekommt Haustuer-Ausgang zum Bauplatz
  if (!num)
    raum->AddExitNoCheck("raus", megamap[owner]);

  return raum;
}

// returnt das Seherhaus-Objekt von Besitzer 'ow'
// nur zum Loeschen oder Wegbewegen daher einfacher als _LadeHaus
object FindeHaus(string ow)
{
  // es wird kein passendes Seherhaus verwaltet
  if (!member(megamap, ow))
    return 0;
  return load_object(HAUSNAME(ow));
}

// Haus von owner loeschen (samt Savefile!). Dieser Vorgang ist unwiderruflich!
int LoescheHaus(string owner)
{
  object haus;
  int rooms;
  string tmp;

  // keine passenden Rechte fuers Loeschen
  if (!secure())
    return -1;

  // Haus-Objekt finden, als Raumautoloader im Bauplatz austragen und entfernen
  haus = FindeHaus(owner);
  if (!haus)
    return -2;
  OBJECTD->RemoveObject(haus, object_name(environment(haus)));
  environment(haus)->RemoveItem(object_name(haus));

  // Raumanzahl merken
  rooms = megamap[owner,HP_ROOMS];

  // Haus aus Verwalter-Mapping loeschen
  megamap = m_delete(megamap, owner);

  // Verwalter-Mapping abspeichern
  save_object(SAVEFILE);

  // Uebergaenge von anderen Seherhaeusern zu diesem entfernen
  check_exits(owner, rooms, 1);

  // Savefile fuer das Haus entfernen
  if (file_size(HAUSSAVEPATH+owner+".o")>0)
    rm(HAUSSAVEPATH+owner+".o");

  // Savefiles fuer die Raeume entfernen
  do {
    if (file_size(tmp = sprintf((HAUSSAVEPATH+"%s%d.o"),owner,rooms))>0)
      rm(tmp);
  } while (--rooms >= 0);

  // Savefile fuer die Truhe loeschen
  if (file_size(HAUSSAVEPATH+owner+"truhe.o")>0)
    rm(HAUSSAVEPATH+owner+"truhe.o");

  // Repfile fuer das Seherhaus loeschen
  // TODO: Eintraege aus ERRORD loeschen.
  if (file_size(PATH+"rep/"+owner+".rep") > 0)
    rm(PATH+"rep/"+owner+".rep");

  // Statistik ueber alle Seherhaeuser erneuern
  dump();
  return 1;
}

// Haus von owner vom Raum 'von' in den Raum 'nach' verschieben.
int VerlegeHaus(string owner, string von, string nach)
{
  object h, ziel;

  // kein Recht das zu tun
  if (!secure())
    return -111;

  // zu verlegendes Haus nicht auffindbar
  if (!(h=FindeHaus(owner)))
    return -1;

  // aktueller Standort des Hauses ist nicht Startpunkt von
  if (object_name(environment(h)) != von)
    return -2;

  // Ziel-Standort nicht ladbar
  catch(ziel = load_object(nach);publish);
  if (!ziel)
    return -3;

  // Am Zielort darf kein Haus gebaut werden
  if (Unbebaubar(ziel))
    return -4;

  // Seherhaus ist mit anderem Seherhaus verbunden und kann daher nicht
  // verschoben werden
  if (check_exits(owner, megamap[owner,HP_ROOMS], 0))
    return -5;

  // neuen Standort in Verwalter-Mapping eintragen
  megamap[owner] = nach;
  // Raumautoloader am alten Standort austragen und am neuen eintragen
  OBJECTD->RemoveObject(h, von);
  OBJECTD->AddObject(h, nach);
  // Haus bewegen
  h->move(nach, M_NOCHECK);
  // Haustuer-Ausgang umtragen und Hauptraum speichern
  catch(RAUMNAME(owner,0)->AddExitNoCheck("raus", nach);publish);
  catch(RAUMNAME(owner,0)->Save();publish);
  // Verwalter-Mapping speichern
  save_object(SAVEFILE);
  // Haus als Inmventar des alten Bauplatzes austragen
  von->RemoveItem(object_name(h));
  // Statistik ueber alle Seherhaeuser erneuern
  dump();

  return 1;
}

// Kann in ob ein Haus gebaut werden? 0: Ja, sonst Fehler!
int Unbebaubar(object ob)
{
  // Raum ist geclont oder hat kein eigenes Filet, z.B. VC
  if (clonep(ob) || file_size(object_name(ob)+".c")<0)
    return 1;

  // Innenraum
  if (({int})ob->QueryProp(P_INDOORS))
    return 2;

  // Bauplatz-Property nicht gesetzt
  if (!(({int})ob->QueryProp(P_HAUS_ERLAUBT)))
    return 3;

  return 0;
}

// Jemandem im Haus Zusatzrechte einraeumen
string *Erlaube(string owner, string *wer)
{
  string *all;

  // es wird kein passendes Seherhaus verwaltet
  if (!member(megamap, owner))
    return 0;

  all = megamap[owner, HP_ALLOWED];
  // fuege wer zu all dazu ohne doppelte Eintraege zu erzeugen:
  all += wer-all;
  // aender Rechteliste in der Verwaltung
  megamap[owner, HP_ALLOWED] = all;
  // speicher Verwalter-Mapping
  save_object(SAVEFILE);
  // return Liste der aktuellen Rechteinhaber
  return all;
}

// Jemandem im Haus Zusatzrechte entziehen
string *Verbiete(string owner, string *wer)
{
  // es wird kein passendes Seherhaus verwaltet
  if (!member(megamap, owner))
    return 0;

  // aender Rechteliste in der Verwaltung
  megamap[owner, HP_ALLOWED] -= wer;
  // speicher Verwalter-Mapping
  save_object(SAVEFILE);
  // return Liste der aktuellen Rechteinhaber
  return megamap[owner, HP_ALLOWED];
}

// Abfrage von Property 'prop' im Haus von 'owner'
// prop gleich HP_ENV, HP_ROOMS oder HP_ALLOWED aus haus.h
mixed HausProp(string owner, int prop)
{
  // es wird kein passendes Seherhaus verwaltet
  if (!member(megamap, owner))
    return 0;

  return megamap[owner,prop];
}

// zerlegt das mapping, fasst indices mit gleichen values zusammen
// und packt das ganze in ein array -> Deduplizieren
// Das Ergebnis enthaelt
// a) fuer jedes Detail ein Array, welches als erstes Element ein Array von
// Schluesseln und als zweites Element einen String enthaelt.
// b) fuer jedes Kommando ein Array, welches als erstes Element ein Array von
// Syntaxen und zwei folgende Elemente mit Strings (Meldungen) enthaelt.
// Beides ist in einem Format, dass die Elemente unmodifiziert an die
// entsprechenden Add...()-Aufrufe gegeben werden koennen.

// ([ "key1" : A, "key2" : B, "key3" : A ])
// => ({ ({ ({ "key1", "key3" }), A )}, ({ ({ "key2" }), B )} })

// ([ "key1" : ([ "key11" : A; B, "key12" : C; B, "key13" : A, B ]),
//    "key2" : ([ "key21" : A; B, "key22" : C; A ]) ])
// => ({ ({ ({ "key1 key11", "key1 key13", "key2 key21" }), A, B }),
//       ({ ({ "key1 key12" }), C, B }),
//       ({ ({ "key2 key22" }), C, A }) })
mixed PCrunch(mapping prop)
{
  mixed ret = ({});
  int done = 0;
  foreach(string key, mixed entry : prop)
  {
    if(mappingp(entry))
    {
      // mapping breite 2 im mapping fuer H_COMMANDS
      foreach(string subkey, string val1, string val2 : entry)
      {
        done = 0;
        foreach(mixed retset : ret)
        {
          // falls es schon im ergebnis-array etwas mit den selben werten gibt,
          // fuege den index dort dazu
          if(sizeof(retset) == 3 && retset[1] == val1 && retset[2] == val2)
          {
            retset[0] += ({ key+" "+subkey });
            done = 1;
            break;
          }
        }
        // falls es noch nix im ergebnis-array mit den selben werten gab,
        // fuege einen neuen eintrag hinzu
        if(!done)
          ret += ({ ({ ({ key+" "+subkey }), val1, val2 }) });
      }
    }
    else
    {
      // einzelne Werte im Mapping fuer P_DETAILS und P_READ_DETAILS
      done = 0;
      foreach(mixed retset : ret)
      {
        // falls es schon im ergebnis-array etwas mit dem selben wert gibt,
        // fuege den index dort dazu
        if(sizeof(retset) == 2 && retset[1] == entry)
        {
          retset[0] += ({ key });
          done = 1;
          break;
        }
      }
      // falls es noch nix im ergebnis-array mit dem selben wert gab,
      // fuege einen neuen eintrag hinzu
      if(!done)
        ret += ({ ({ ({ key }), entry }) });
    }
  }
  return ret;
}
