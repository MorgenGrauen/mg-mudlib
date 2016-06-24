/*  /obj/tools/merkzettel.c
    Merkzettel fuer Magier
    Autor: Zesstra
    ggf. Changelog:
*/
#pragma strict_types
#pragma no_shadow
#pragma range_check
#pragma pedantic

inherit "/std/secure_thing";

#include <properties.h>
#include <language.h>
#include <wizlevels.h>
#include <moving.h>
#include <defines.h>

#define NEED_PROTOTYPES
#include "merkzettel.h"
#undef NEED_PROTOTYPES

//Variablen
mapping notizen;
//mapping notizen_inaktiv;
int maxusedID;
static string owner; //UID des Besitzers, wird in P_AUTOLOAD gespeichert
//static int *cache; //nach Prioritaet sortiertes Array der IDs

void create() {
  ::create();
  SetProp(P_NAME, "Merkzettel");
  SetProp(P_SHORT, "Ein Merkzettel");
  AddId(({"merkzettel","zettel"}));
  //Absicht, damit die Blueprint eine ID, P_NAME und P_SHORT hat.
  if(!clonep(this_object())) return;
  
  SetProp(P_LONG,
  "Was wolltest Du gleich nochmal schoenes proggen? ;-)\n"
  "Befehlsliste:\n"
  "zmerke/znotiz <string>  : Etwas auf den Merkzettel schreiben\n"
  "zliste                  : 20 wichtigsten Notizen anzeigen\n"
  "zliste <int>            : <int> wichtigste Notizen anzeigen\n"
  "                          (unwichtigste, wenn <int> negativ)\n"
  "                          Angabe von 0 oder 'alle' gibt alle aus\n"
  "zliste <int> <filter>   : <int> wichtigste Notizen anzeigen\n"
  "                          (Filter: 'aktiv', 'wartend'\n"
  "                           'inaktiv', 'abgehakt', 'alle'|'0')\n"
  "                          Ohne Angabe eines Filter gilt 'aktiv'\n"
  "zstatus <int>           : Status von Notiz mit ID umschalten\n"
  "zstatus <int> <int2>    : Status von Notiz mit ID auf <int> setzen \n"
  "zloesche/zrm <int>      : Notiz mit ID <int> loeschen\n"
  "zlies/zlese <int>       : Notiz mit ID <int> anzeigen\n"
  "zdeplies <int> <int2>   : Abhaengigkeiten von <int> lesen, \n"
  "                        : rekursiv, wenn <int2> != 0\n"
  "zfertig/zhake <int>     : Notiz mit ID <int> abhaken (fertig)\n"
  "zprio <int1> <int2>     : Notiz mit ID <int> mit Prioritaet <int2>\n"
  "                          versehen.\n"
  "zdep <int1> <int2>      : Notiz mit ID <int> soll von Notiz mit ID\n"
  "                          <int2> abhaengen.\n"
  "zhelfer <int1> <namen>  : Notiz mit ID <int> wird (mit)bearbeitet\n"
  "                          von den angegebenen Personen\n"
  "zergaenze <int> <text>  : haengt <text> an Notiz <int> an\n"
  "zersetze <int> <text>   : ersetzt Text von Notiz <int> durch <text>\n"
  "zexpire <int>           : alle abgehakten Notizen archivieren und loeschen,\n"
  "                        : die aelter als <int> Tage sind.\n"
  "zzeige <name>           ; zeige <name>, wie furchtbar viel Du zu tun hast\n"
  "zwedel <name>	   : <name> aus der Ferne vor der Nase rumwedeln.\n"
  );
  SetProp(P_INFO,break_string("Ein Merkzettel fuer Magier. ;-)."));
  SetProp(P_GENDER, MALE);
  SetProp(P_ARTICLE, 1);
  SetProp(P_WEIGHT, 0);
  SetProp(P_VALUE, 0);
  Set(P_AUTOLOADOBJ,#'query_autoloadobj,F_QUERY_METHOD);
  Set(P_AUTOLOADOBJ,#'set_autoloadobj,F_SET_METHOD);
  Set(P_AUTOLOADOBJ,PROTECTED,F_MODE_AS);
  SetProp(P_NEVERDROP,1);
  SetProp(P_NODROP,1);
  AddCmd(({"zmerke","znotiz"}),"AddNotiz");
  AddCmd(({"zliste","ztodo"}),"ListNotizen");
  AddCmd(({"zloesche","zrm"}),"RemoveNotiz");
  AddCmd(({"zfertig","zhake"}),"FinishNotiz");
  AddCmd(({"zlies","zlese"}),"LiesNotiz");
  AddCmd(({"zprio"}),"ChangePrio");
  AddCmd(({"zdep"}),"ChangeDep");
  AddCmd(({"zstatus"}),"ChangeStatus");
  AddCmd(({"zersetze"}),"ErsetzeText");
  AddCmd(({"zergaenze"}),"ErgaenzeText");
  AddCmd(({"zhelfer","zdelegiere"}),"ChangeHelper");
  AddCmd(({"zzeige"}),"ZeigeZettel");
  AddCmd(({"zwedel"}),"WedelZettel");
  AddCmd(({"zdeplies"}),"LiesDeps");
  AddCmd(({"zexpire"}),"Expire");
  //cache=({});
  
}

//****************** AddCmd *****************************
int AddNotiz(string str) {
  
  if(!objectp(TI)) return(0);
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
  //ggf. nur Einlesen, Cache muss hier nicht aktualisiert werden ;-)
  checkStatus(1);
  str=(string)PL->_unparsed_args(0);  //kein parsing
  if (!stringp(str) || !sizeof(str)) {
    tell_object(PL,
      "Was moechtest Du Dir denn notieren?\n");
    return(1);
  }
  if (sizeof(str)>MAX_NOTE_LENGTH) {
    tell_object(PL,BS(
      sprintf("Deine Notiz ist mit %d Zeichen zu lang! Leider passen auf "
             "Deinen Merkzettel nur % Zeichen drauf.",
        sizeof(str),MAX_NOTE_LENGTH)));
    return(1);
  }
  
  //m_allocate gilt nur bis Funktionsende, daher hilft das in
  //checkStatus leider nicht.
  if (!mappingp(notizen) || !sizeof(notizen))
    notizen=m_allocate(0,7);
  
  //freie ID finden (maxusedID+1 sollte die naechste frei sein, aber falls
  //da mal was kaputtgegangen ist: Pruefen ;-) )
  while(member(notizen,maxusedID)) {maxusedID++;}
  //Reihenfolge: s. merkzettel.h
  notizen+=([maxusedID:str;0;time();({});({});NOTE_ACTIVE;0]);
/*  notizen+=([maxusedID]);
  notizen[maxusedID,NOTE_TEXT]=str;
  notizen[maxusedID,NOTE_PRIO]=0;
  notizen[maxusedID,NOTE_STARTTIME]=time();
  notizen[maxusedID,NOTE_DEPS]=({});
*/
  
  //cache=({}); //cache invalidieren
  //in Kuerze speichern, aber nicht fuer jede Aenderung speichern
  save_me(60);
  tell_object(PL,BS(
      sprintf("Du schreibst sorgfaeltig Deine Notiz auf Deinen Merkzettel. "
          "Es ist nun die %d. Notiz.",maxusedID)));
  tell_room(environment(TI),
    BS(sprintf("%s notiert sich etwas auf %s Merkzettel.\n",
        capitalize((string)TI->name(WER)),
        (((int)TI->QueryProp(P_GENDER))==FEMALE ? "ihrem" : "seinem") 
              )),({TI}));
  return(1);
}

int LiesNotiz(string str) {
  int id;
  
  if(!objectp(TI)) return(0);
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
  if (!stringp(str) || !sizeof(str) || !id=to_int(str)) {
    tell_object(PL,
      "Welche Notiz moechtest Du lesen? Bitte eine ID angeben!\n");
    return(1);
  }
  if (!checkStatus()) {
    tell_object(PL,
      "Es gibt keine Notiz, die Du lesen koenntest!\n");
    return(1);
  }
  if (!member(notizen,id)) {
    tell_object(PL,
      sprintf("Es gibt keine Notiz mit der ID: %d\n",id));
    return(1);
  }
  //Ausgabetext holen und ausgeben
  tell_object(PL,sprintf("\nDu vertiefst Dich in Deinen Merkzettel und liest:\n%s\n",_LiesNotiz(id)));
  tell_room(environment(TI),
    BS(sprintf("%s liest etwas auf %s Merkzettel.\n",
        capitalize((string)TI->name(WER)),
        (((int)TI->QueryProp(P_GENDER))==FEMALE ? "ihrem" : "seinem") 
              )),({TI}));
  return(1);
}

int RemoveNotiz(string str) {
  int id;
  
  if(!objectp(TI)) return(0);
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
  if (!stringp(str) || !sizeof(str) || !id=to_int(str)) {
    tell_object(PL,
      "Welche Notiz moechtest Du ausradieren? Bitte eine ID angeben!\n");
    return(1);
  }
  if (!checkStatus()) {
    tell_object(PL,
      "Es gibt keine Notiz, die Du ausradieren koenntest!\n");
    return(1);
  }
  if (!member(notizen,id)) {
    tell_object(PL,
      sprintf("Es gibt keine Notiz mit der ID: %d\n",id));
    return(1);
  }
  //direkt Loeschen, keine Kopie, daher m_delete()
  m_delete(notizen,id);
  //in Kuerze speichern, aber nicht fuer jede Aenderung speichern
  save_me(60);
  //cache invalidieren
  //cache=({});
  tell_object(PL,
    sprintf("Sorgfaeltig radierst Du Notiz %d von Deinem Merkzettel.\n",
           id));
  tell_room(environment(TI),
    BS(sprintf("%s radiert sorgfaeltig etwas von %s Merkzettel.\n",
        capitalize((string)TI->name(WER)),
        (((int)TI->QueryProp(P_GENDER))==FEMALE ? "ihrem" : "seinem") 
              )),({TI}));
  return(1);
}

int FinishNotiz(string str) {
  int id;
  mixed liste;
  if(!objectp(TI)) return(0);
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
  if (!stringp(str) || !sizeof(str) || !id=to_int(str)) {
    tell_object(PL,
      "Welche Notiz moechtest Du abhaken? Bitte eine ID angeben!\n");
    return(1);
  }
  if (!checkStatus()) {
    tell_object(PL,
      "Es gibt keine Notiz, die Du abhaken koenntest!\n");
    return(1);
  }
  if (!member(notizen,id)) {
    tell_object(PL,
      sprintf("Es gibt keine Notiz mit der ID: %d\n",id));
    return(1);
  }
  if(notizen[id,NOTE_STATUS]==NOTE_FINISHED) {
    tell_object(PL,
      sprintf("Notiz %d ist schon abgehakt!\n",id));
    return(1);
  }
  liste=getUnresolvedDeps(id);  //liste ist Array von ints
  if (sizeof(liste)) {
    liste=map(liste,#'to_string);  //liste ist Array von strings
    tell_object(PL,BS(sprintf("Du kannst Notiz %d nicht abhaken, da noch "
        "nicht erledigte Abhaengigkeiten existieren. IDs: %s.",
        id,CountUp(liste))));
    return(1); 
  }
  notizen[id,NOTE_STATUS]=NOTE_FINISHED;
  notizen[id,NOTE_CLOSETIME]=time();
  tell_object(PL,BS(
    sprintf("Du malst zufrieden einen schoenen grossen Haken hinter "
        "die Notiz Nr. %d. Hach - was bist Du nun zufrieden! :-)\n",id)));
  tell_room(environment(TI),
    BS(sprintf("%s hakt erfreut etwas auf %s Merkzettel ab.\n",
        capitalize((string)TI->name(WER)),
        (((int)TI->QueryProp(P_GENDER))==FEMALE ? "ihrem" : "seinem") 
              )),({TI}));
  return(1);
}

int ListNotizen(string str) {
  int zahl,notizzahl,i,id,filterstate;
  string txt;
  string *arr;
  status invers;
  
  if(!objectp(TI)) return(0);
  str=(string)PL->_unparsed_args(0);
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
  if (!notizzahl=checkStatus()) {
    tell_object(PL,BS(
      "Dein Merkzettel ist bluetenweiss und leer! Wie kommt das denn?? "
      "Hast Du nix zu tun?"));
    return(1);
  }
  //Argumente parsen
  if(stringp(str) && sizeof(str))
    arr=explode(str," ");
  if (pointerp(arr)) arr-=({""}); //doppelte leerzeichen im String
  if (pointerp(arr) && sizeof(arr)>=2) {
    zahl=to_int(arr[0]);
    switch(arr[1]) {
      case "aktiv":
        filterstate=NOTE_ACTIVE; break;
      case "inaktiv":
        filterstate=NOTE_INACTIVE; break;
      case "wartend":
      case "pending":
        filterstate=NOTE_PENDING; break;
      case "abgehakt":
      case "fertig":
      case "abgeschlossen":
        filterstate=NOTE_FINISHED; break;
      case "alle":
        filterstate=0; break;
      default: 
        filterstate=to_int(arr[1]);
        break;
    }
  }
  else if (pointerp(arr) && sizeof(arr)==1) {
    zahl=to_int(arr[0]);
    filterstate=NOTE_ACTIVE;
  }
  else {
    //Voreinstellungen
    zahl=20;
    filterstate=NOTE_ACTIVE;
  }
  //1. Argument "all" oder "alle" ist: alle anzeigen
  if (pointerp(arr) && sizeof(arr) && stringp(arr[0]) &&
      sizeof(arr[0]) && arr[0]=="alle")
    zahl=notizzahl;  //alle anzeigen
  
  //wenn die gewuenschte Zahl die Anzahl gespeicherter
  //Notizen ueberschreitet (oder 0 ist)
  if(!zahl || zahl>notizzahl) zahl=notizzahl;
  
  //wenn 1. Argument negativ: Liste in umgekehrter Reihenfolge
  if (zahl<0) {
    invers=1;
    zahl=zahl*-1;
  }
  txt=sprintf("\n|%:9|s|%:56|s|%:7|s|\n","ID","Notiztext","Prio.");
  txt+=sprintf("%:78'-'s\n","-");
  //alle passenden Notizen ermitteln
  arr=filter(m_indices(notizen),#'note_filter,filterstate);
  //sortieren
  arr=sort_array(arr,#'sort_prio);
  if (zahl>sizeof(arr)) zahl=sizeof(arr);
  //ausgeben
  for(i=0;i<zahl;i++) {
    if (!invers)
      id=to_int(arr[i]);
    else
      //von hinten
      id=to_int(arr[<i+1]); // i+1 ist wirklich Absicht. ;-)
    txt+=sprintf("| %:7|d | %:54-s | %:5|d |\n",
            id,
	    //nur bis zum ersten \n anzeigen
	    explode(notizen[id,NOTE_TEXT],"\n")[0],
            notizen[id,NOTE_PRIO]);
  }
  txt+=sprintf("%:78'-'s\n\n","-");
  tell_object(PL,txt);
  tell_room(environment(TI),
    BS(sprintf("%s wirft einen Blick auf %s Merkzettel.\n",
        capitalize((string)TI->name(WER)),
        (((int)TI->QueryProp(P_GENDER))==FEMALE ? "ihren" : "seinen") 
              )),({TI}));
  return(1);
}

int ChangeDep(string str) {
  int id;
  string *arr;
  int *deps;
  
  notify_fail("Bitte eine ID und eine min. eine Abhaengigkeit angeben!\n");
  if(!objectp(TI)) return(0);
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
  str=(string)PL->_unparsed_args(0);
  if (!stringp(str) || !sizeof(str))
    return(0);
  
  if (!checkStatus()) {
    tell_object(PL,BS(
      "Dein Merkzettel ist bluetenweiss und leer! Wie kommt das denn?? "
      "Hast Du nix zu tun?"));
    return(1);
  }
  arr=explode(str," ");
  arr-=({""});  // "" entfernen, wichtig. ;-)
  if (sizeof(arr)<2)
    return(0);
  id=to_int(arr[0]);
  arr-=({arr[0]});  //eigene ID loeschen
  if (sizeof(arr)<1)
    return(0);  //mehrfach die eigene ID gilt nicht.;-)
  if (!member(notizen,id)) {
    tell_object(PL,
      sprintf("Es gibt keine Notiz mit der ID: %d\n",id));
    return(1);
  }
  //dies updated direkt die Abhaengigkeiten in 'notizen'!
  map(arr,#'update_deps,id);
  save_me(60);
  tell_object(PL,BS(
      sprintf("Du aenderst die Abhaengigkeiten von Notiz %d.\n",
              id)));
  tell_room(environment(TI),
    BS(sprintf("%s kritzelt auf %s Merkzettel herum.\n",
        capitalize((string)TI->name(WER)),
        (((int)TI->QueryProp(P_GENDER))==FEMALE ? "ihrem" : "seinem") 
              )),({TI}));
  return(1);
}

int ChangePrio(string str) {
  int id, prio;
  string* arr;
  
  notify_fail("Bitte eine ID und eine neue Prioritaet angeben!\n");
  if(!objectp(TI)) return(0);
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
  str=(string)PL->_unparsed_args(0);
  if (!stringp(str) || !sizeof(str))
    return(0);
  
  if (!checkStatus()) {
    tell_object(PL,BS(
      "Dein Merkzettel ist bluetenweiss und leer! Wie kommt das denn?? "
      "Hast Du nix zu tun?"));
    return(1);
  }
  arr=explode(str," ");
  if (sizeof(arr)>2) {
    notify_fail("Bitte nur eine ID und eine neue Prioritaet angeben!");
    return(0);
  }
  else if(sizeof(arr)==2) {
    id=to_int(arr[0]);
    prio=to_int(arr[1]);
  }
  else
    return(0);

  if (!member(notizen,id)) {
    tell_object(PL,
      sprintf("Es gibt keine Notiz mit der ID: %d\n",id));
    return(1);
  }
  notizen[id,NOTE_PRIO]=prio;
  //cache=({}); //cache invalidieren
  save_me(60);
  tell_object(PL,BS(
      sprintf("Du aenderst die Prioritaet von Notiz %d auf %d.",
              id,notizen[id,NOTE_PRIO])));
  tell_room(environment(TI),
    BS(sprintf("%s kritzelt auf %s Merkzettel herum.\n",
        capitalize((string)TI->name(WER)),
        (((int)TI->QueryProp(P_GENDER))==FEMALE ? "ihrem" : "seinem") 
              )),({TI}));
  return(1);
}

int ChangeHelper(string str) {
  int id;
  string *arr;
  
  notify_fail("Bitte eine ID und eine min. einen Namen angeben!\n");
  if(!objectp(TI)) return(0);
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
  str=(string)PL->_unparsed_args(0);
  if (!stringp(str) || !sizeof(str))
    return(0);
  
  if (!checkStatus()) {
    tell_object(PL,BS(
      "Dein Merkzettel ist bluetenweiss und leer! Wie kommt das denn?? "
      "Hast Du nix zu tun?"));
    return(1);
  }
  arr=explode(str," ");
  arr-=({""});  // "" entfernen, wichtig. ;-)
  if (sizeof(arr)<2)
    return(0);
  id=to_int(arr[0]);
  arr-=({arr[0]});  //eigene ID loeschen
  if (sizeof(arr)<1)
    return(0);  //mehrfach die eigene ID gilt nicht.;-)
  if (!member(notizen,id)) {
    tell_object(PL,
      sprintf("Es gibt keine Notiz mit der ID: %d\n",id));
    return(1);
  }
  notizen[id,NOTE_HELPER]=arr;
  save_me(60);
  tell_object(PL,BS(
      sprintf("Mitarbeiter von Notiz %d geaendert.",id)));
  tell_room(environment(TI),
    BS(sprintf("%s kritzelt auf %s Merkzettel herum.\n",
        capitalize((string)TI->name(WER)),
        (((int)TI->QueryProp(P_GENDER))==FEMALE ? "ihrem" : "seinem") 
              )),({TI}));
  return(1);
}

int ChangeStatus(string str) {
  int id, state;
  string *arr;
  
  notify_fail(BS(
      "Bitte min. eine ID angeben, um den Status umzuschalten oder "
      "eine ID und den neuen Status (1==Aktiv, 2==Wartend, "
      "-1==Inaktiv/Pause)"));
  if(!objectp(TI)) return(0);
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
  str=(string)PL->_unparsed_args(0);
  if (!stringp(str) || !sizeof(str))
    return(0);
  if (!checkStatus()) {
    tell_object(PL,BS(
      "Dein Merkzettel ist bluetenweiss und leer! Wie kommt das denn?? "
      "Hast Du nix zu tun?"));
    return(1);
  }
  arr=explode(str," ");
  if (sizeof(arr)>2) {
    notify_fail("Bitte nur eine ID und einen neuen Status angeben!");
    return(0);
  }
  else if(sizeof(arr)==2) {
    id=to_int(arr[0]);
    state=to_int(arr[1]);
  }
  else if(sizeof(arr)==1) {
    id=to_int(arr[0]);
  }
  else
    return(0);
  
  if (!member(notizen,id)) {
    tell_object(PL,
      sprintf("Es gibt keine Notiz mit der ID: %d\n",id));
    return(1);
  }
  if(member(NOTE_STATES,state)==-1) {
    if (notizen[id,NOTE_STATUS]==NOTE_ACTIVE) {
      tell_object(PL,BS(
          sprintf("%d ist ein unbekannter Status, setzt Notiz %d auf "
              "'Inaktiv'.",state,id)));
      state=NOTE_INACTIVE;
    }
    else {
      tell_object(PL,BS(
          sprintf("%d ist ein unbekannter Status, setzt Notiz %d auf "
              "'Aktiv'.",state,id)));
      state=NOTE_ACTIVE;
    }
  }
  notizen[id,NOTE_STATUS]=state;
  save_me(60);
  tell_object(PL,BS(
      sprintf("Status von Notiz %d geaendert.",id)));
  tell_room(environment(TI),
    BS(sprintf("%s kritzelt auf %s Notizzettel herum.\n",
        capitalize((string)TI->name(WER)),
        (((int)TI->QueryProp(P_GENDER))==FEMALE ? "ihrem" : "seinem") 
              )),({TI}));
  return(1);
}

int ErsetzeText(string str) {
  string *arr;
  int id;
  
  notify_fail("Bitte eine ID und einen neuen Text angeben!\n");
  if(!objectp(TI)) return(0);
  
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
  str=(string)PL->_unparsed_args(0);
  if (!stringp(str) || !sizeof(str))
    return(0);
  
  if (!checkStatus()) {
    tell_object(PL,BS(
      "Dein Merkzettel ist bluetenweiss und leer! Wie kommt das denn?? "
      "Hast Du nix zu tun?"));
    return(1);
  }
  arr=explode(str," ");
  arr-=({""});  // "" entfernen
  if (sizeof(arr)<2)
    return(0);
  id=to_int(arr[0]);
  str=implode(arr[1..]," ");  //text wiederherstellen, ohne erstes Element
  if (!sizeof(str)) return(0);
  if (!member(notizen,id)) {
    tell_object(PL,
      sprintf("Es gibt keine Notiz mit der ID: %d\n",id));
    return(1);
  }
  notizen[id,NOTE_TEXT]=str;
  save_me(60);
  tell_object(PL,BS(
      sprintf("Text von Notiz %d ersetzt.",id)));
  tell_room(environment(TI),
    BS(sprintf("%s radiert zuerst etwas auf %s Notizzettel herum und "
        "schreibt anschliessend sorgfaeltig etwas neues auf.\n",
        capitalize((string)TI->name(WER)),
        (((int)TI->QueryProp(P_GENDER))==FEMALE ? "ihrem" : "seinem") 
              )),({TI}));
  return(1);
}

int ErgaenzeText(string str) {
  string *arr;
  int id;
  
  notify_fail("Bitte eine ID und einen Text angeben!\n");
  if(!objectp(TI)) return(0);
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
  str=(string)PL->_unparsed_args(0);
  if (!stringp(str) || !sizeof(str))
    return(0);
  
  if (!checkStatus()) {
    tell_object(PL,BS(
      "Dein Merkzettel ist bluetenweiss und leer! Wie kommt das denn?? "
      "Hast Du nix zu tun?"));
    return(1);
  }
  arr=explode(str," ");
  arr-=({""});  // "" entfernen
  if (sizeof(arr)<2)
    return(0);
  id=to_int(arr[0]);
  str="\n";
  str+=implode(arr[1..]," ");  //text wiederherstellen
  if (!member(notizen,id)) {
    tell_object(PL,
      sprintf("Es gibt keine Notiz mit der ID: %d\n",id));
    return(1);
  }
  notizen[id,NOTE_TEXT]+=str;
  save_me(60);
  tell_object(PL,BS(
      sprintf("Text von Notiz %d ergaenzt.",id)));
  tell_room(environment(TI),
    BS(sprintf("%s ergaenzt etwas auf %s Notizzettel.\n",
        capitalize((string)TI->name(WER)),
        (((int)TI->QueryProp(P_GENDER))==FEMALE ? "ihrem" : "seinem") 
              )),({TI}));
  return(1);
}

int ZeigeZettel(string str) {
    //anderen zeigen, wie furchtbar viel man zu tun hat.
  object pl;
  int count;
  int *ids;
  
  notify_fail("Wem willst Du Deinen Merkzettel zeigen?\n");
  if (!objectp(TI)) return(0);
  if (!environment() || !environment(environment()))
     return(0);
  if (!stringp(str) || !sizeof(str)) return(0);
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
 if (!checkStatus()) {
    tell_object(PL,BS(
      "Dein Merkzettel ist bluetenweiss und leer! Wie kommt das denn?? "
      "Hast Du nix zu tun? Du willst anderen Leuten doch bestimmt nicht "
      "zeigen, dass Du nix zu tun hast, oder?"));
    return(1);
  }
 if(!objectp(pl=present(str,environment(environment()))))
     return(0);
 if (!living(pl))
     return(0);

 ids=filter(m_indices(notizen),#'note_filter,NOTE_ACTIVE);
 tell_object(TI,BS("Du zeigst "+pl->Name(WEM)+" voller Stolz deinen vollen "
       "Merkzettel."));
 tell_room(environment(environment()),BS(sprintf(
	 "%s zeigt %s %s Merkzettel.",
       TI->Name(WER),pl->Name(WEM),TI->QueryPossPronoun(MALE,WEN,SINGULAR))),
     ({TI,pl}));
 switch(sizeof(ids)) {
     case 0..10:
       tell_object(pl,BS(sprintf("%s zeigt Dir voller Stolz %s Merkzettel. Du "
	       "wirfst einen schnellen Blick darueber und zaehlst %d "
	       "Notizen. Na, soviel ist das ja zum Glueck noch nicht.",
	       TI->Name(WER),TI->QueryPossPronoun(MALE,WEN,SINGULAR),
	       sizeof(ids))));
       break;
    case 11..20:
        tell_object(pl,BS(sprintf("%s zeigt Dir voller Stolz %s Merkzettel. Du "
	       "wirfst einen schnellen Blick darueber und zaehlst %d "
	       "Notizen. Oh, das ist ja schon so einiges!",
	       TI->Name(WER),TI->QueryPossPronoun(MALE,WEN,SINGULAR),
	       sizeof(ids))));
       break;
    default:
        tell_object(pl,BS(sprintf("%s zeigt Dir voller Stolz %s Merkzettel. Du "
	       "wirfst einen schnellen Blick darueber und zaehlst %d "
	       "Notizen. Puuuh. %s hat ganz schoen viel zu tun! In "
	       "Dir regt sich leises Mitleid.",
	       TI->Name(WER),TI->QueryPossPronoun(MALE,WEN,SINGULAR),
	       sizeof(ids),TI->Name(WER) )));
       break;
 }
 return(1);
 }

int WedelZettel(string str) {
    object pl;
    string rwedel;
  //wedelt anderen mit dem Zettel vor der Nase herum.
  notify_fail("Wem willst Du Deinen Merkzettel zeigen?\n");
  if (!objectp(TI)) return(0);
  if (!environment() || !environment(environment()))
     return(0);
  if (!stringp(str) || !sizeof(str)) return(0);
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
  if (!objectp(pl=find_player(str))) return(0);
  if (!present(pl,environment(environment())))
    rwedel="aus der Ferne";
      
  tell_object(PL,BS("Du wedelst "+pl->Name(WEM) 
	+ (rwedel?" "+rwedel:"") 
	+" heftig mit Deinem Merkzettel vor der Nase herum."));
  tell_object(pl,BS(PL->Name(WER)+ " wedelt Dir " 
	+ (rwedel?rwedel+" ":"") +"heftig mit "
	+PL->QueryPossPronoun(MALE,WEM,SINGULAR)
	+" Merkzettel vor der Nase herum."));
  tell_room(environment(pl),BS(PL->Name(WER) + " wedelt "
	+pl->Name(WEM) + (rwedel?" "+rwedel:"") + " heftig mit "
       +PL->QueryPossPronoun(MALE,WEM,SINGULAR)
       +" Merkzettel vor der Nase herum."),({PL,pl}));
  return(1);
}

int LiesDeps(string str) {
  //erstmal (rekursiv) alle Abhaengigkeiten einer Notiz ermitteln
  string *arr;
  int id, rec, i;
  int *liste;
  
  notify_fail("Bitte eine ID und ggf. '1' fuer rekursive Suche angeben!\n");
  if(!objectp(TI)) return(0);
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
  str=(string)PL->_unparsed_args(0);
  if (!stringp(str) || !sizeof(str))
    return(0);
  
  if (!checkStatus()) {
    tell_object(PL,BS(
      "Dein Merkzettel ist bluetenweiss und leer! Wie kommt das denn?? "
      "Hast Du nix zu tun?"));
    return(1);
  }
  arr=explode(str," ");
  arr-=({""});  // "" entfernen
  if (sizeof(arr)==1) {
    id=to_int(arr[0]);
    rec=0;  //Standard: keine Rekursion
  }
  else if (sizeof(arr)>=2) {
    id=to_int(arr[0]);
    rec=to_int(arr[1]);
  }
  else {
    return(0);
  }
  if (!member(notizen,id)) {
    tell_object(PL,
      sprintf("Es gibt keine Notiz mit der ID: %d\n",id));
    return(1);
  }
  //Notizen, von denen id abhaengt, holen
  //und nur aktive Notizen behalten
  liste=filter(getDeps(id,rec),#'note_filter,NOTE_ACTIVE);
  //nach Prioritaet sortieren
  liste=sort_array(liste,#'sort_prio);
  //dann mal ausgeben.
  i=sizeof(liste);
  tell_object(PL,BS(sprintf("Du vertiefst Dich in die Abhaenhigkeiten von Notiz: %d\n",id)));
  while(i--) {
    tell_object(PL,_LiesNotiz(liste[i])+"\n");
  }
  tell_room(environment(TI),
    BS(sprintf("%s liest etwas auf %s Merkzettel.\n",
        capitalize((string)TI->name(WER)),
        (((int)TI->QueryProp(P_GENDER))==FEMALE ? "ihrem" : "seinem") 
              )),({TI}));
  return(1);    
}

int Expire(string str) {
  float age;
  int sekunden, i;
  int *liste;
  string res;
  
  notify_fail("Bitte angeben, wie alt zu loeschende Notizen sein sollen (in Tagen)!\n");
  if(!objectp(TI)) return(0);
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
  str=(string)PL->_unparsed_args(0);
  if (!stringp(str) || !sizeof(str))
    return(0);
  age=to_float(str);
  if (age<=0) return(0);
  sekunden=time() - (int)(age*86400);
  if (!checkStatus()) {
    tell_object(PL,BS(
      "Dein Merkzettel ist bluetenweiss und leer! Wie kommt das denn?? "
      "Hast Du nix zu tun?"));
    return(1);
  }
  //abgehakte Notizen ermitteln
  liste=filter(m_indices(notizen),#'note_filter,NOTE_FINISHED);
  //Notizen ermitteln, die lang genug abgehakt sind.
  liste=filter(liste,#'aelter_als,sekunden);
  if (!sizeof(liste)) {
    tell_object(PL,BS("Keine Notizen gefunden, die lang genug abgehakt sind."));
    return(1);
  }
  res=sprintf("Archivierte Notizen vom %s\n\n",dtime(time()));
  for (i=sizeof(liste);i--;) {
    //Infos noch einmal ausgeben und loeschen
    res+=_LiesNotiz(liste[i])+"\n";
    m_delete(notizen,liste[i]);
  }
  res+="\n";
  write_file(ARCHIVE(owner),res);
  tell_object(PL,BS(sprintf(
    "%d alte Notizen wurden nach %s archiviert und geloescht.",
       sizeof(liste),ARCHIVE(owner))));
  return(1);
}

//
static string _LiesNotiz(int id) {
  //Funktion verlaesst sich drauf, dass id existiert. ;-)
  string res,txt;
  res=BSL(sprintf("%-11s: %d","Notiz-Nr.",id));
  res+=break_string(notizen[id,NOTE_TEXT],78,"Notiztext  : ",
                             BS_INDENT_ONCE|BS_LEAVE_MY_LFS);
  res+=sprintf("%-11s: %d\n","Prioritaet",notizen[id,NOTE_PRIO]);
  //wenn nicht aktiv: Status anzeigen
  if (notizen[id,NOTE_STATUS]!=NOTE_ACTIVE) {
    switch(notizen[id,NOTE_STATUS]) {
      case NOTE_INACTIVE:
          txt="Inaktiv/Pause";
          break;
      case NOTE_FINISHED:
          txt="Abgeschlossen"; 
          break;
      case NOTE_PENDING:
          txt="Warte auf Rueckmeldung"; 
          break;
      default: txt=sprintf("%d",notizen[id,NOTE_STATUS]); break;
    }
    res+=sprintf("%-11s: %s\n","Status",txt);
    //wenn abgeschlossen: Endezeit anzeigen
    if (notizen[id,NOTE_STATUS]==NOTE_FINISHED)
      res+=sprintf("%-11s: %s\n","Endezeit",
                  dtime(notizen[id,NOTE_CLOSETIME]));
  }
  res+=sprintf("%-11s: %s\n","Merkzeit",dtime(notizen[id,NOTE_STARTTIME]));
  //nur wenn Abhaengigkeiten: anezeigen
  if (pointerp(notizen[id,NOTE_DEPS]) && sizeof(notizen[id,NOTE_DEPS])) {
    txt=implode(map(notizen[id,NOTE_DEPS],#'to_string)," ");
    res+=sprintf("%-11s: %s\n","abh. von",txt);
  }
  //nur wenn Helfer: anezeigen
  if (pointerp(notizen[id,NOTE_HELPER]) && sizeof(notizen[id,NOTE_HELPER]))
    res+=sprintf("%-11s: %s\n","Helfer",
                implode(notizen[id,NOTE_HELPER]," "));
  return(res);
}

//int. Helfer
static int note_filter(int id,int filterstate) {
    //kein filter -> passt immer
    if (!filterstate) return(1);
    if (member(notizen,id) &&
    notizen[id,NOTE_STATUS]==filterstate)
    return(1);
    return(0);
}

static int aelter_als(int id,int zeit) {
  //liefert 1 zurueck, wenn Schliesszeit der Notiz laenger als 'sekunden' her ist, sonst 0.
  if (notizen[id,NOTE_CLOSETIME]<zeit) return(1);
  return(0);
}

static string update_deps(string str,int id) {
  //wandelt str nach int um
  //schaut, ob dep positiv und noch nicht bekannt ist, wenn ja: 
  //zu deps hinzufuegen
  //wenn dep < 0 und bekannt ist: aus deps entfernen
  //Rueckgabewert ist egal. ;-)
  int dep;
  dep=to_int(str);
  if (dep>0 && member(notizen[id,NOTE_DEPS],dep)==-1)
    notizen[id,NOTE_DEPS]+=({dep});
  else if (dep<0 && member(notizen[id,NOTE_DEPS],dep*-1)!=-1)
    notizen[id,NOTE_DEPS]-=({dep*-1});
  return("");
}

static int *getDeps(int id, int rec) {
  //ermittelt (ggf. rekursiv bei rec!=0) die Abhaengigkeiten einer Notiz
  int i, *liste;
  liste=notizen[id,NOTE_DEPS];
  //jetzt ggf. noch rekursiv weiterholen
  if (rec)  {
    i=0;
    while(i<sizeof(liste)) {
        //nicht schreien. ;-)
        if (get_eval_cost()<100000) {
            tell_object(PL,"Fehler: Zu lange Rekursion bei "
		"Ermitteln der Abhaengigkeiten, Abbruch\n");
            return(liste);
        }
        //Abhaengigkeiten der anderen Abhaengigkeiten hinten anhaengen.
        liste+=notizen[liste[i],NOTE_DEPS];
        i++;
    }
  }
  return(liste);
}

static int *getUnresolvedDeps(int id) {
  //liefert die Anzahl der nicht abgehakten Notizen, von denen eine 
  //Notiz abhaengig ist (rekursiv)
  int *liste;
  //Abhaengigkeiten rekursiv holen
  liste=getDeps(id,1);
  //nicht abgehakte zurueckgeben
  return(liste-filter(liste,#'note_filter,NOTE_FINISHED));
}

void init() {
  ::init();
  //wenn kein Env, Env kein magier oder nicht der Eigentuemer ist: weg
  if (!objectp(environment()) || !IS_LEARNER(environment())
     || (stringp(owner) && getuid(environment())!=owner) ) {
    if (find_call_out("remove") == -1)
      call_out("remove",1,1);
    return;
  }
  //beim erstmaligen Bewegen in einen Magier wird der als Eigentuemer
  //registriert. Danach kommt diese Info aus P_AUTOLOAD
  if (!stringp(owner))
    owner=getuid(environment());
}

static nomask status check_allowed() {
  //Zugriff auf die Daten fuer den Eigentuemer
  if(objectp(this_interactive()) &&
     getuid(this_interactive())==owner
     && !process_call())
    return(1);
  return(0);
}

protected int restore_me() {
  //laedt Savefile im Home vom Magier
  if(!stringp(owner)) return(0);
  if(!restore_object(SAVEFILE(owner))) {
    maxusedID=1;
    notizen=0;
    //cache=({});
    return(0);
  }
  return(1);
}

varargs protected void save_me(int delay) {
  //speichert Savefile im Home vom Magier
  if(!stringp(owner)) return;
  //wenn maxusedID==0 wurde der zettel noch nicht ordentlich initialisiert
  //bzw. restauriert. In diesem Fall wuerde ein leeres Savefile geschrieben
  if (maxusedID==0)
    restore_me();
  if(!delay)
    //direkt speichen. ;-)
    save_object(SAVEFILE(owner));
  else {
    //verzoegert speichern, wenn schon ein call_out laeuft, nehm ich den
    if(find_call_out(#'save_me)==-1)
      call_out(#'save_me,delay);
  }
}

varargs int remove(int silent) {
  //erstmal speichern. ;-)
  save_me();
  return ::remove(silent);
}

static string query_autoloadobj() {
  //in P_AUTOLOAD wird nur der Eigentuemer gespeichert
  return owner;
}

static string set_autoloadobj(mixed arg)  {
  //Eigentuemer aus P_AUTOLOAD restaurieren
  if (stringp(arg))
    owner=arg;
  return(owner);
}

//Shadow unerwuenscht
int query_prevent_shadow() {return(1);}

varargs static int checkStatus(int nocache) {
  //schaut, ob Notizen da, ob der Cache Ok ist, etc.
  //liefert Anzahl der Notizen auf dem Merkzettel
  
  //wenn keine Notizen da sind, schauen, ob das Savefile eingelesen werden
  //kann
  if (!mappingp(notizen) || !sizeof(notizen))
    restore_me();
  if (!mappingp(notizen)) return(0);
/* Cache ist eigentlich nicht noetig und mir gerade zu unpraktisch, raus damit.  
  //wenn keine Aktualisierung des Cache erwuenscht ist
  if(nocache) return(0);
  
  if (sizeof(notizen)) {
    if(!pointerp(cache) || !sizeof(cache)) {
      //cache erneuern
      cache=sort_array(m_indices(notizen),"sort_prio");
    }
  }
  return(sizeof(cache)); //aequivalent zu sizeof(notizen)
  */
  return(sizeof(notizen));
}

static status sort_prio(int key1, int key2) {
  // 1, falls falsche Reihenfolge,
  // 0, wenn richtige
  // und zwar _absteigend_, also hoechste Prio in liste[0] !
  return(notizen[key1,NOTE_PRIO]<=notizen[key2,NOTE_PRIO]);
}


//*******************************************************

// Debugging
mapping QueryNotizen() {
  
  if(!check_allowed()) {
    if(objectp(this_interactive()))
      tell_object(this_interactive(),
        BS("Dieser Zugriff auf den Merkzettel wurde nicht erlaubt."));
    return(0);
  }
  checkStatus();
  return(notizen);
}

//int *QueryCache() {return(cache);}

int QuerySize() {return(sizeof(notizen));}

string QueryOwner() {return(owner);}
