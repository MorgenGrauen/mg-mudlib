#pragma strict_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma verbose_errors
#pragma combine_strings
#pragma rtt_checks
#pragma pedantic
#pragma warn_deprecated

#include "/secure/config.h"
#include "/secure/wizlevels.h"

#define MAX_ROOMS_PER_LIST 10

// die div. Speicherorte und Pfade
#define SAVEFILE    "/secure/ARCH/potions"
#define TIPS(x)     "/secure/ARCH/ZT/"+x
#define ORAKEL      "/room/orakel"
#define POTIONTOOL  "/obj/tools/ptool"

// Fuer die Dump-Funktion
#define POTIONDUMP "/secure/ARCH/POTIONS.dump"
#define DUMP(str)   write_file(POTIONDUMP, str)

// Modifikationen loggen. "event" ist eins der Events aus der Liste:
// ADD_POTION, ACTIVATE, DEACTIVATE, MODIFY_PATH, MODIFY_LISTNO
#define LOGFILE_MOD     "/log/ARCH/POTIONS_MOD.log"
#define MODLOG(event,num,data) write_file(LOGFILE_MOD, \
  sprintf("%17s %-14s %-3d sEUID %s %s\n", \
    strftime("%Y-%b-%d %R"), event, num, secure_euid(), data) )

// Indizierungs-Konstanten fuer das potions-Mapping
#define POT_ROOMNAME 0
#define POT_LISTNO   1

// Konstanten fuer div. Rueckgabewerte. Keine 0, da das eine gueltige ZT-ID
// sein kann und abfragende Objekte verwirren koennte.
#define POT_IS_ACTIVE            1
#define POT_ACCESS_DENIED       -1
#define POT_WRONG_DATATYPE      -2
#define POT_NO_SUCH_ROOM        -3
#define POT_ALREADY_REGISTERED  -4
#define POT_INVALID_POTION      -5
#define POT_NO_SUCH_FILE        -6
#define POT_INVALID_LIST_NUMBER -7
#define POT_ALREADY_ACTIVE      -8
#define POT_ALREADY_INACTIVE    -9
#define POT_NOT_INACTIVE       -10
#define POT_NOT_ACTIVE         -11

// Zaehler fuer den als naechsten anzulegenden ZT
private int nextroom;

// Liste aller ZTs einschl. inaktive, ([ int num : string room; int list ])
private mapping potions = ([]);

// Liste der inaktiven ZTs, ({ int num })
private int *inactive = ({});

// Cache mit den einzelnen Listen, ([ int list : int *potionlist ])
private nosave mapping lists;

// reverse_table Lookup Cache, ([string room: int number])
private nosave mapping reverse_table = ([]);

// Cache fuer die bereits von der Platte eingelesenen ZTs, um Plattenzugriffe
// insbesondere beim Erzeugen der Tipliste durch das Orakel zu reduzieren.
private nosave mapping tipmap = ([]);

int ActivateRoom(string room);

private int secure() {
  return (!process_call() && ARCH_SECURITY);
}

mixed QueryPotionData(int num) {
  if ( !secure() )
    return POT_ACCESS_DENIED;
  return ([num : potions[num,0]; potions[num,1] ]);
}

int *QueryInactivePotions() {
  return copy(inactive);
}

private void RebuildCache() {
  // Cache invalidieren; vor-initialisiert zur Beschleunigung des Rebuilds.
  lists = ([0:({}),1:({}),2:({}),3:({}),4:({}),5:({}),6:({}),7:({})]);
  foreach (int num, string room, int list : potions) {
    reverse_table += ([room:num]);
    lists[list] += ({num});
  }
  return;
}

int QueryActive(mixed potion) {
  if ( extern_call() && !secure() )
    return POT_ACCESS_DENIED;
  int ret;
  // Wenn nach dem Pfad des ZTs gefragt wird, diesen zuerst in die Nummer
  // umwandeln durch Lookup im reverse_table Cache
  if ( stringp(potion) ) {
    potion = reverse_table[potion];
  }
  // Ein ZT ist aktiv, wenn er in der Liste potions steht und nicht in der
  // Liste der inaktiven ZTs (inactive) steht. Dann dessen Nummer
  // zurueckgeben; inaktive zuerst pruefen, weil die inaktiven auch in 
  // "potions" enthalten sind und somit alle ZTs ausser den ungueltigen
  // als aktiv gemeldet wuerden.
  if ( member(inactive,potion)>-1 )
    ret = POT_NOT_ACTIVE;
  else if ( member(potions,potion) && potions[potion,POT_LISTNO]!=-1 )
    ret = potion;
  // ansonsten ist das kein gueltiger ZT
  else
    ret = POT_INVALID_POTION;
  return ret;
}

private void save_info() {
  save_object(SAVEFILE);
}

protected void create() {
  seteuid(getuid(this_object()));
  if ( !restore_object(SAVEFILE) ) {
    // Fehler ausgeben, damit es jemand merkt. Dennoch wird jetzt im Anschluss
    // der Cache neu aufgebaut (damit die Datenstrukturen gueltig sind).
    catch(raise_error("Potionmaster: no/corrupt savefile! Reinitializing.\n"); publish);
  }
  RebuildCache();
}

int AddPotionRoom(string room, int list) {
  if ( !secure() )
    return POT_ACCESS_DENIED;
  // Neuer Raum muss ein gueltiger String sein.
  if ( !stringp(room) )
    return POT_WRONG_DATATYPE;
  if ( !intp(list) || list<0 || list>7 )
    return POT_WRONG_DATATYPE;

  // Pfad mit Hilfe des Masters vervollstaendigen (+, ~ etc. ersetzen)
  room=(string)master()->_get_path(room,0);

  // Datei mit dem ZT-Spruch muss existieren.
  if ( file_size( TIPS(to_string(nextroom)+".zt") ) < 0 ) {
    raise_error("Potionmaster: Tipfile missing, please create "+
      to_string(nextroom)+".zt");
    return POT_NO_SUCH_FILE;
  }
  // Neuer Raum darf noch nicht in der Liste enthalten sein.
  if ( member(m_values(potions,POT_ROOMNAME), room)!=-1)
    return POT_ALREADY_REGISTERED;
  // Neuer Raum muss ladbar sein.
  if ( catch(load_object(room); publish) )
    return POT_NO_SUCH_ROOM;

  // Jetzt kann's endlich losgehen, Raum eintragen, nextroom hochzaehlen
  potions += ([nextroom : room; list]);
  MODLOG("ADD_POTION", nextroom, room);
  // Neu eingetragene ZTs werden auch gleich aktiviert; ActivateRoom()
  // baut den Cache selbst neu auf, daher kann das hier entfallen.
  ActivateRoom(room);
  nextroom++;
  save_info();
  return nextroom;
}

int ChangeRoomPath(string old, string new) {
  if ( !secure() )
    return POT_ACCESS_DENIED;

  // beide Pfade muessen gueltige Strings sein
  if ( !stringp(old) || !stringp(new) )
    return POT_WRONG_DATATYPE;

  // Pfad mit Hilfe des Masters vervollstaendigen (+, ~ etc. ersetzen)
  old=(string)master()->_get_path(old,0);
  new=(string)master()->_get_path(new,0);

  // Der neue Raum darf nicht bereits eingetragen sein, ...
  if ( member(reverse_table,new) )
    return POT_ALREADY_REGISTERED;
  // ... und der alte Raum muss noch eingetragen sein.
  if ( !member(reverse_table,old) )
    return POT_NO_SUCH_ROOM;
  // Neuer Raum muss ladbar sein.
  if (catch(load_object(new);publish))
    return POT_NO_SUCH_ROOM;

  // Aktuelle ZT-Nummer des alten Pfades ermitteln
  int num = reverse_table[old];
  // Pfad aendern, Cache neubauen und Savefile speichern
  potions[num,POT_ROOMNAME] = new;
  RebuildCache();
  save_info();
  MODLOG("MODIFY_PATH", num, old+" => "+new);
  return num;
}

int ActivateRoom(string room) {
  if ( !secure() )
    return POT_ACCESS_DENIED;
  // Aktuelle ZT-Nummer ermitteln. Etwas umstaendlich, da im Fehlerfall -1
  // benoetigt wird.
  int num = member(reverse_table,room) ? reverse_table[room] : -1;
  // Nummer muss existieren
  if ( num == -1 )
    return POT_INVALID_POTION;
  // ZT ist nicht inaktiv, dann kann man ihn auch nicht aktivieren.
  if ( member(inactive, num)==-1 )
    return POT_ALREADY_ACTIVE;
  inactive -= ({num});
  RebuildCache();
  save_info();
  MODLOG("ACTIVATE", num, room);
  return num;
}

int SetListNr(string room, int list) {
  if ( !secure() )
    return POT_ACCESS_DENIED;
  // Aktuelle ZT-Nummer ermitteln. Etwa umstaendlich, da im Fehlerfall -1
  // benoetigt wird.
  int num = member(reverse_table,room) ? reverse_table[room] : -1;
  // Nummer muss existieren
  if ( num == -1 )
    return POT_INVALID_POTION;
  // Listennummer muss zwischen 0 und 7 liegen.
  if ( list < 0 || list > 7 )
    return POT_INVALID_LIST_NUMBER;

  // alte Nummer aufschreiben zum Loggen.
  int oldlist = potions[num,POT_LISTNO];
  // Listennummer in der ZT-Liste aktualisieren.
  potions[num,POT_LISTNO] = list;
  RebuildCache();
  save_info();
  MODLOG("MODIFY_LISTNO", num, oldlist+" -> "+list);
  return num;
}

int DeactivateRoom(string room) {
  if ( !secure() )
    return POT_ACCESS_DENIED;
  // Aktuelle ZT-Nummer ermitteln. Etwa umstaendlich, da im Fehlerfall -1
  // benoetigt wird.
  int num = member(reverse_table,room) ? reverse_table[room] : -1;
  // Nummer muss existieren
  if ( num == -1 )
    return POT_INVALID_POTION;
  // ZT darf nicht bereits inaktiv sein
  if ( member(inactive,num)>-1 )
    return POT_ALREADY_INACTIVE;
  inactive += ({num});
  RebuildCache();
  save_info();
  MODLOG("DEACTIVATE", num, room);
  return num;
}

private int *_create_list(int listno, int anz) {
  int *list = ({});
  // Listenerzeugung lohnt nur dann, wenn mind. 1 Eintrag gefordert wird und
  // die Listennummer im gueltigen Bereich zwischen 0 und 7 ist.
  if ( anz>0 && listno>=0 && listno<=7 ) {
    int *tmp = lists[listno] - inactive;
    // Wenn die Listengroesse maximal genauso gross ist wie die angeforderte
    // Anzahl, kann diese vollstaendig uebernommen werden.
    if ( sizeof(tmp) <= anz ) {
      list = tmp;
    } else { // ansonsten soviele Eintraege erzeugen wie angefordert
      foreach(int i: anz) {
        int j=random(sizeof(tmp));
        list += ({tmp[j]});
        tmp -= ({tmp[j]});
      }
    }
  }
  return list;
}

mixed *InitialList() {
  mixed *list=({});
  foreach(int i : 8)
    list+=_create_list(i,MAX_ROOMS_PER_LIST);
  return list;
}

// Aufrufe aus den Spielershells und dem Potiontool sind erlaubt
int HasPotion(object room) {
  if ( !query_once_interactive(previous_object()) && 
       load_name(previous_object()) != POTIONTOOL )
    return POT_ACCESS_DENIED;
  return objectp(room) ? reverse_table[object_name(room)] : POT_NO_SUCH_ROOM;
}

// Listennummer ermitteln, in der der ZT num enthalten ist.
// Auch inaktive ZTs werden als zu einer Liste gehoerig gemeldet.
int GetListByNumber(int num) {
  return member(potions,num) ? potions[num,POT_LISTNO] : POT_INVALID_POTION;
}

// Listennummer ermitteln, in der der inaktive ZT num enthalten ist.
// Da alle Zaubertraenke in einer Gesamtliste enthalten sind, kann direkt das
// Resultat von GetListByNumber() zurueckgegeben werden.
int GetInactListByNumber(int num) {
  if ( member(inactive,num) > -1 )
    return GetListByNumber(num);
  else
    return POT_NOT_INACTIVE;
}

// Wird nur von /obj/tools/ptool aufgerufen. Wenn der Aufruf zugelassen wird,
// erfolgt von dort aus ein ChangeRoomPath(), und dort wird bereits geloggt.
// Erzmagier duerfen auch aufrufen.
mixed GetFilenameByNumber(int num) {
  if ( extern_call() && 
       object_name(previous_object()) != POTIONTOOL &&
       !ARCH_SECURITY )
    return POT_ACCESS_DENIED;
  if ( !member(potions, num) )
    return POT_INVALID_POTION;
  return potions[num,POT_ROOMNAME];
}

// Wird vom Spielerobjekt gerufen; uebergeben werden der Raum, der initial
// FindPotion() aufrief, die ZT-Liste des Spielers sowie die Liste bereits
// gefundener ZTs.
//
// In std/player/potion.c ist sichergestellt, dass room ein Objekt ist und
// dass die Listen als Int-Arrays uebergeben werden.
//
// Es werden aus historischen Gruenden (noch) beide ZT-Listen uebergeben,
// aber es muss nur knownlist ueberprueft werden, da diese eine Teilmenge
// von potionlist ist und somit jeder ZT in ersterer auf jeden Fall auch
// in letzterer enthalten sein _muss_.
int InList(object room, int *potionlist, int *knownlist) {
  if ( !query_once_interactive(previous_object()) && !secure() )
    return 0; //POT_ACCESS_DENIED;
  int num = QueryActive(object_name(room));
  
  // Zunaechst keinen Fehlercode zurueckgeben, weil das Spielerobjekt
  // damit im Moment noch nicht umgehen kann.
  if ( num < 0 )
    return 0; //POT_INVALID_POTION;

  return (member(knownlist,num)>-1);
}

// Wird vom Spielerobjekt gerufen, um einen gefundenen ZT aus dessen ZT-
// Listen austragen zu lassen. Das Spielerobjekt stellt sicher, dass room
// ein Objekt ist, und dass [known_]potionrooms Arrays sind.
//
// ACHTUNG! Sowohl potionrooms, als auch known_potionrooms sind die
//          Originaldaten aus dem Spielerobjekt, die hier ALS REFERENZ
//          uebergeben werden! Vorsicht bei Aenderungen an der Funktion!
//
// Wenn std/player/potions.c geaendert wird, ist diese Funktion obsolet
// => ggf. komplett rauswerfen. Dann kann auch der Fehlercode in InList()
// reaktiviert und deren Parameter korrigiert werden
void RemoveList(object room, int* potionrooms, int* known_potionrooms) {
  // ZT ist aktiv, das wurde bereits in InList() geprueft, das vor dem
  // Aufruf von RemoveList() aus dem Spielerobjekt gerufen wird. Daher reicht
  // es aus, die ZT-Nummer aus dem reverse_table Lookup zu holen.
  int num = reverse_table[object_name(room)];
  int tmp = member(potionrooms, num);
  potionrooms[tmp] = -1;
  tmp = member(known_potionrooms, num);
  known_potionrooms[tmp] = -1;
  return;
}

#define LISTHEADER "################## Liste %d ################## (%d)\n\n"
#define INACT_HEADER "################## Inaktiv ################## (%d)\n\n"

int DumpList() {
  if ( !secure() )
    return POT_ACCESS_DENIED;
  // Zuerst die Caches neu aufbauen, um sicherzustellen, dass die Listen
  // aktuell sind.
  RebuildCache();
  // Dumpfile loeschen
  rm(POTIONDUMP);
  // Alle Listen der Reihe nach ablaufen. Es wird in jedem Schleifendurchlauf
  // einmal auf die Platte geschrieben. Das ist Absicht, weil es speicher-
  // technisch sehr aufwendig waere, die Ausgabedaten stattdessen erst in
  // einer Variablen zu sammeln und dann wegzuschreiben.
  foreach(int *listno : sort_array(m_indices(lists),#'>)) {
    // Zuerst den Header der ensprechenden Liste schreiben.
    DUMP(sprintf(LISTHEADER, listno, sizeof(lists[listno])));
    // Dann alle Potions der Liste durchgehen
    foreach(int potion : lists[listno]) {
      // Wenn der ZT inaktiv ist, ueberspringen wir den.
      if ( member(inactive,potion)>-1 )
        continue;
      // Raumpfad aus der Gesamtliste holen.
      string str = potions[potion,POT_ROOMNAME];
      // Wenn der nicht existiert, entsprechenden Hinweis dumpen, ansonsten
      // den Raumnamen.
      //ZZ: ich finde ja, wir sollten sicherstellen, dass das nicht mehr
      //passiert. Ist das mit dem AddPotionRoom oben nicht schon so?
      if ( !stringp(str) || !sizeof(str) )
        str="KEIN RAUM ZUGEORDNET!!!";
      DUMP(sprintf("%3d. %s\n", potion, str));
    }
    // 2 Leerzeilen zwischen jeder Gruppe einfuegen
    DUMP("\n\n");
  }
  // Zum Schluss den Header der Inaktiv-Liste schreiben.
  DUMP(sprintf(INACT_HEADER, sizeof(inactive)));
  // Dann alle inaktiven Potions ablaufen
  foreach(int i : inactive) {
    //ZZ: sonst muesste man hier wohl auch auf den fehlenden Raum pruefen.
    DUMP(sprintf("%3d. (Liste %d) %s\n", i, GetListByNumber(i),
      potions[i,POT_ROOMNAME]));
  }
  return 1;
}

//ORAKEL-Routinen

/* Aufbau eines Tips:

   Tralala, lalala
   Dideldadeldum
   Huppsdiwupps
   XXXXX
   Noch ein zweiter Tip
   Dingeldong
   %%%%%

   Die Prozentzeichen sind das Endezeichen, ab hier koennen eventuelle
   Kommentare stehen. Die 5 X sind das Trennzeichen zwischen zwei Tips
   zum selben ZT.
*/

// TIPLOG() derzeit unbenutzt
//#define LOGFILE_READ    "ARCH/POTIONS_TIP_READ.log"
//#define TIPLOG(x)      log_file(LOGFILE_READ, x)

mixed TipLesen(int num) {
  string *ret = tipmap[num];
  //Funktion darf nur vom Orakel und EM+ gerufen werden.
  if ( previous_object() != find_object(ORAKEL) && !secure() )
    return POT_ACCESS_DENIED;
  // Derzeit kein Log, da diese Informationen tendentiell eher 
  // uninteressant sind.
  // Timestamp ist vom Format "2012-Okt-03 14:18"
  /*TIPLOG(sprintf("%s ZT-Tip gelesen: %d von TP: %O, PO: %O, TI: %O\n",
    strftime("%Y-%b-%d %R"), num, this_player(), previous_object(),
    this_interactive()));*/

  // ZT-Spruch ist bereits im Tip-Cache enthalten, dann direkt ausgeben.
  if ( pointerp(ret) )
    return ret;
  
  // ZT-Spruch auf grundlegende syntaktische Korrektheit pruefen:
  // explode() liefert ein Array mit 2 Elementen, wenn die Ende-Markierung
  // vorhanden und somit das File diesbezueglich korrekt aufgebaut ist.
  string *tip=explode( read_file(TIPS(num+".zt"))||"", "%%%%%" );
  if (sizeof(tip) >= 2) {
    // Der erste Eintrag in dem Array ist dann der Spruch. Wenn dieser
    // die Trenn-Markierung enthaelt, entstehen hier 2 Hinweise, ansonsten
    // bleibt es bei einem.
    tipmap[num] = explode(tip[0], "XXXXX\n");
    return tipmap[num];
  }
  return POT_NO_SUCH_FILE;
}

// Datenkonvertierung alte Alists => Mappings
/*mapping ConvertData() {
  //if ( !secure()) ) return;
  // all_rooms ist eine Alist der Form ({ string *room, int *number })
  // als 3. Eintrag schreiben wir ueberall -1 rein, das wird spaeter durch die
  // Listennummer des betreffenden ZTs ersetzt und erlaubt einen einfachen
  // Check auf Datenkonsistenz.
  potions = mkmapping( all_rooms[1],
                       all_rooms[0],
                       allocate(sizeof(all_rooms[0]), -1) );
  // Alle Listen ablaufen
  foreach(int *list: active_rooms ) {
    // Alle ZTs dieser Liste ablaufen und die entsprechende Listennummer in
    // der neuen Datenstruktur setzen.
    foreach(int num: list) {
      potions[num,POT_LISTNO] = member(active_rooms,list);
    }
  }
  // inactive_rooms ist eine Alist der gleichen Form. Die Schleife addiert
  // einfach alle Eintraege darin auf, so dass ein Int-Array entsteht, das
  // alle inaktiven ZT-Nummern enthaelt; das allerdings nur fuer Listen mit
  // mindestens einem Element.
  foreach(int *list : inactive_rooms ) {
    if ( sizeof(list) ) {
      inactive += list;
      // Alle ZTs dieser Liste ablaufen und die entsprechende Listennummer in
      // der neuen Datenstruktur setzen.
      foreach(int num : list) {
        potions[num,POT_LISTNO] = member(inactive_rooms,list);
      }
    }
    // unify array
    inactive = m_indices(mkmapping(inactive));
  }
  mapping invalid = ([ POT_INVALID_LIST_NUMBER : ({}) ]);
  walk_mapping(potions, function void (int pnum, string path, int listno) {
    if ( listno == -1 )
      invalid[POT_INVALID_LIST_NUMBER] += ({pnum});
  });
  return sizeof(invalid[POT_INVALID_LIST_NUMBER]) ? invalid : potions;
}*/

