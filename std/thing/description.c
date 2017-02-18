// MorgenGrauen MUDlib
//
// thing/description.c -- description handling for standard objects
//
// $Id: description.c 9561 2016-05-25 19:33:22Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#include <tls.h>
#include <thing/description.h>
#include <thing/material.h>
#include <thing/lighttypes.h>
#include <exploration.h>                  // wegen EPMASTER
#include <class.h>

#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <thing/language.h>

#undef NEED_PROTOTYPES
#include <properties.h>

// Variable um den FP abzuspeichern 
private nosave mixed *explore;

// Prototypen
public string short();
public varargs string long(int mode);

//                       #####################
//######################## System-Funktionen ############################
//                       #####################

// Objekt erzeugen
protected void create()
{
  string poid, tpid;
  object tp;

  SetProp( P_NAME, "Ding" );
  SetProp( P_SHORT, "Nichts besonderes" );
  SetProp( P_LONG, 0 );

  Set( P_ADJECTIVES, ({}) );
  Set( P_NAME_ADJ, ({}) );
  Set( P_IDS, ({}) );
  Set( P_CLASS, ({}) );

  Set( P_READ_DETAILS, ([]), F_VALUE);
  Set( P_READ_DETAILS, SECURED|NOSETMETHOD, F_MODE_AS);

  Set( P_DETAILS, ([]), F_VALUE);
  Set( P_DETAILS, SECURED|NOSETMETHOD, F_MODE_AS );

  Set( P_SMELLS, ([]), F_VALUE);
  Set( P_SMELLS, SECURED|NOSETMETHOD, F_MODE_AS );

  Set( P_SOUNDS, ([]), F_VALUE);
  Set( P_SOUNDS, SECURED|NOSETMETHOD, F_MODE_AS );

  Set( P_TOUCH_DETAILS, ([]), F_VALUE);
  Set( P_TOUCH_DETAILS, SECURED|NOSETMETHOD, F_MODE_AS );

  // Aenderungen an dieser Prop sind tabu.
  Set( P_CLONE_TIME, NOSETMETHOD|SECURED, F_MODE_AS );

  // Id des Cloners und des Besitzers kommen nach P_CLONER
  if (objectp( tp=this_interactive()||this_player() ))
  {
    tpid=geteuid(tp);
    if (!(tpid=geteuid(tp))) tpid=getuid(tp);
  }
  else
    tpid="UNKNOWN";

  if (previous_object())
  {
    if (!(poid = geteuid(previous_object())))
      poid = getuid(previous_object());
  }
  else
    poid="UNKNOWN";

  Set( P_CLONER, (poid != tpid ? poid+":"+tpid: tpid) );
  Set( P_CLONER, NOSETMETHOD|SECURED, F_MODE_AS );

  // Gibt es FPs ?
  explore = (mixed *)EPMASTER->QueryExplore();

  return;
}

protected void create_super() {
  set_next_reset(-1);
}

//                        ##################
//######################### Forscherpunkte ##############################
//                        ##################

// FP vergeben
static void GiveEP( int type, string key )
{
  //Abbruch, wenn vergebendes Objekt schon zerstoert ist. ACHTUNG: Auch
  //diese Abfrage wuerde kein FP vergeben werden, wenn sich das Objekt im
  //vergebenden Kommando zerstoert, da der Driver call_other() von zerstoerten
  //Objekten ignoriert!
  if (!objectp(this_object())) return;
  if (this_player()) this_player()->countCmds( type, key );
  
  if (explore&&!extern_call()&&
      (explore[0] == type) && (member(explore[1], key) >= 0) )
    EPMASTER->GiveExplorationPoint(key);
  return;
}

// Manche Objekte koennen mit rename_object einen neuen Filenamen bekommen.
// Danach sollte der EPMASTER neu nach den Details befragt werden.
void __reload_explore()
{
  explore = (mixed *)EPMASTER->QueryExplore();
  return;
}

//                         #################
//########################## ID-Management ##############################
//                         #################

// Gibt eine ID zurueck, die den Ladenamen, die aktuelle Kurzbeschreibung und
// die aktuelle Langbeschreibung beruecksichtigt. Diese ID ist ein Hashwert.
public string description_id() {
  return hash(TLS_HASH_MD5, load_name() + short() + long());
}

/* Ids muessen uebergeben werden, da unit machmal mit plural-Ids, */
/* also anderen als den normalen arbeiten muss. */
int match_item( string str, string *ids )
{
  string *obj,*ads;
  int len, i;
  
  // Parameter falsch? Passt nicht ...
  if(!str)           return 0;
  if(!pointerp(ids)) return 0;
  if(!sizeof(ids))   return 0;
  
  // Ist schon so dabei? Na Klasse :-)
  if(member(ids,str)>-1) return 1;
  
  // Keine Adjektive vorhanden ... passt nicht.
  if (!(ads=QueryProp(P_ADJECTIVES))) return 0;
  if (!sizeof(ads))                   return 0;
  
  // Nur ein Wort? Dann passt es nicht
  obj=explode(str," ");
  if (!(len=sizeof(obj)-1)) return 0;
  
  // Adjektive stehen am Anfang. Sobald es nicht mehr passt,
  // muss es das Objektiv sein.
  while(i<len&&member(ads,obj[i])>-1) i++;

  return (member(ids,implode(obj[i..len]," "))>-1);
}

// Wird vom Gamedriver aufgerufen (present)
// Hat dieser Gegenstand die ID str?
// lvl wird ignoriert
varargs int id( string str, int lvl ) 
{ 
  string str2, tmp;
  int count;	
  string|string* ids;

  // Kein Argument? Dann passt es nicht ...
  if (!stringp(str)) return 0;

  // Keine IDs? Auch nicht gut ...
  if (!pointerp(ids=QueryProp(P_IDS))) return 0;
  if (!sizeof(ids)) return 0;

  ids += ({ ("\n" + object_name()),
            ("\n" + explode(object_name(),"#")[0]) });

  // Id passt? Alles klar :-)
  if (match_item( str, ids )) return 1;

  // Die id hat eine Zahl drin. Wenn Zahl die Rohid passt,
  // dann gucken, ob man selber das nte Element ist.
  if (sscanf( str, "%s %d%s", str2, count, tmp)<2) return 0;
  if (count<1) return 0;
  if (sizeof(tmp)) return 0;
  if (!match_item( str2, ids )) return 0;
  if (!environment()) return 0;
  return present(str2,count,environment())==this_object();
}

// Gleich eine ganze Liste von ids testen
int match_ids(string *list)
{
  string *ids;

  // Ungueltige Parameter? Weg hier ...  
  if (!pointerp(list)) return 0;
  if (!pointerp(ids=QueryProp(P_IDS))) return 0;

  ids += ({ ("\n" + object_name()),
            ("\n" + explode(object_name(),"#")[0]) });

  return sizeof( list & ids );
}

// ID hinzufuegen
void AddId( string|string* str )
{
  if (stringp(str)) str = ({ str });
  if (pointerp(str))
    // Doppelte eliminieren
    Set( P_IDS, Query(P_IDS, F_VALUE)-str+str, F_VALUE);
  return;
}

// ID entfernen
void RemoveId(string|string* str)
{
  if (stringp(str)) str = ({ str });
  if (pointerp(str))
    Set(P_IDS,Query(P_IDS, F_VALUE)-str, F_VALUE);
  return;
}

// Alle Ids auf einmal setzen
static string* _set_ids( string* ids )
{
    Set( P_IDS,({}));
    AddId(ids);
    return Query(P_IDS, F_VALUE);
}

// Adjektiv hinzufuegen
void AddAdjective(string|string* str)
{
  if (stringp(str)) str = ({ str });
  if (pointerp(str))
    // Doppelte eliminieren
      Set( P_ADJECTIVES, Query(P_ADJECTIVES, F_VALUE)-str+str, F_VALUE );
  return;
}

// Adjektiv entfernen
void RemoveAdjective(string|string* str)
{
  if (stringp(str)) str = ({ str });
  if (pointerp(str))
    Set( P_ADJECTIVES, Query(P_ADJECTIVES, F_VALUE) - str, F_VALUE );
  return;
}

// Alle Adjektive auf einmal setzen
static string* _set_adjectives(string* adjectives)
{
  Set( P_ADJECTIVES,({}), F_VALUE);
  AddAdjective(adjectives);
  return Query(P_ADJECTIVES, F_VALUE);
}


//                         ################
//########################## Namensgebung ###############################
//                         ################

// Im Fall von mehreren Adjektiven muessen diese mit komma
// zusamengebaut werden, dazu muss ich das leerzeichen aber erstmal
// abschneiden und so weiter ...
private string depointer_adj( <string|string*>* adj, int casus, int demon ) {
  string msg;
  int start;
  string res,a;
	adj = map( adj, #'DeclAdj, casus, demon );
  start = 1;
  res = "";
  foreach( a: adj ) {
    res += (start ? "" : ", ") + a[0..<2];
    start = 0;
  }
  return res + " ";
}

// Wie lautet der Name des Objekts?
varargs string name(int casus,int demon)
{
  mixed sh, adj;
  int art, plural;

  art = QueryProp(P_ARTICLE);

  // RAW: direkt zurueckgeben ohne Verarbeitung
  if (casus == RAW )
  {
    if(pointerp(QueryProp(P_NAME)))
      return QueryProp(P_NAME)[WER];
    return QueryProp(P_NAME);
  }

  // Unsichtbar: Etwas
  if (QueryProp(P_INVIS))
    return ({ "etwas", "von etwas", "etwas", "etwas" })[casus];

  // Kein Name? Schade ...
  if (!(sh=QueryProp(P_NAME)) ||
      (stringp(sh) && !sizeof(sh))) return 0;

  // P_NAME pruefen.
  if (pointerp(sh) && sizeof(sh) != 4)
      raise_error(sprintf("Ungueltige Arraygroesse in P_NAME: %d\n",
            sizeof(sh)));

  // Plural .. Namen verwursten
  if (plural = QueryProp(P_PLURAL))
  {
    // Selber Artikel suchen ist nicht ...
    if (demon==2||!art) demon = 0;

    // falls P_NAME ein Array mit Faellen enthaelt, den richtigen
    // extrahieren...
    if (pointerp(sh)) {
        sh = sh[casus];
    }
    else {
        // sonst versuchen, zu deklinieren.
        int last = sh[<1];
        if (casus == WEM&&last!='s'&&last!='n') sh = sh + "n";
    }

    // Sind Adjektive vorhanden?
    if ( pointerp(adj = QueryProp(P_NAME_ADJ)) && sizeof(adj))
        adj = depointer_adj(adj,casus,demon);
    if (!stringp(adj)) adj = "";

    return sprintf("%s%s%s%s",QueryArticle(casus,demon,0),adj,
                   (plural < 2 ? "":(plural < 8 ?
                    ({ "zwei ", "drei ", "vier ", "fuenf ", "sechs ",
                       "sieben " })[plural-2] : to_string(plural)+" ")),sh);
  }

  // Name ist Pointer: Einfach den richtigen auswaehlen
  if (pointerp(sh))
    sh = sh[casus];

  // Ansonsten doch wieder verwursten ...
  else if (stringp(sh))
  {
    int last = sh[<1];

    switch(casus)
    {
      case WEM:
      case WEN:
        if ( art && last=='e'&&QueryProp(P_GENDER) == MALE)
          sh = (string)sh + "n";
        break;

      case WESSEN:
        if( !art )
        {
          switch(last)
          {
            case 'x':
            case 's':
            case 'z':
              sh = (string)sh + "'";
              break;

          default:
            sh = (string)sh + "s";
          }
        } 
        else
        {
          switch(last)
          {
            default:
              if (QueryProp(P_GENDER)!=FEMALE)
                sh=(string)sh+"s";
              break;
            case 'e':
              if (QueryProp(P_GENDER)==MALE)
                sh=(string)sh+"n";
            case 'x':
            case 's':
            case 'z':
              break;
          } /* switch (last) */
        } /* if( !art ) else */
    } /* switch( casus ) */
  } /* pointerp(sh) */

  // RAW? Dann mal zurueck
  if (demon == RAW) return (string)sh;

  // Selber Artikel suchen ...
  if (demon==2)
  {
    if (art)
      demon = SuggestArticle();
    else
      demon=0; // Kein Artikel: egal (SuggestArticle ist zeitaufwendig)
  }

  if (pointerp(adj = QueryProp(P_NAME_ADJ)) && sizeof(adj))
    adj = depointer_adj(adj,casus,demon);

  if (!stringp(adj))  adj = "";

  return QueryArticle( casus, demon )+adj+sh;
}

// Grossgeschriebenen Namen zurueckgeben
varargs string Name( int casus, int demon )
{
    return capitalize(name( casus, demon )||"");
}

// Langbeschreibung anzeigen
public varargs string long(int mode)
{
    return process_string( QueryProp(P_LONG) );
}

// Kurzbeschreibung anzeigen, falls nicht unsichtbar
public string short()
{
  string sh;

  // Unsichtbar? Dann gibts nichts zu sehen ...
  if (QueryProp(P_INVIS)||!(sh=QueryProp(P_SHORT)))
    return 0;
  
  return process_string(sh)+".\n";
}

// Namens-Adjektive setzen
static <string|string*>* _set_name_adj(string|<string|string*>* adjectives)
{
  if (!adjectives)
      adjectives=({});
  // In Array umwandeln
  else if ( !pointerp(adjectives)) 
      adjectives = ({ to_string(adjectives) });
  return Set( P_NAME_ADJ, adjectives );
}

//                   ############################
//#################### Details, Gerueche, Laerm #########################
//                   ############################

// Low-level Funktion zum Ergaenzen von Details, wird von den div. Add...()
// gerufen, die das richtige Mapping uebergeben.
// Aendert das Mapping <details> direkt.
private void _add_details(string|string* keys,
                          string|string*|mapping|closure descr,
                          mapping details )
{
  if (stringp(keys))
    details[lower_case(keys)]=descr;
  else if (pointerp(keys))
  {
    foreach(string key : keys)
      details[lower_case(key)]=descr;
  }
  else
    raise_error("Wrong type to argument 1, expected string|string*.\n");
}

// Low-level Funktion zum Entfernen von Details, wird von den div. Remove...()
// gerufen, die das richtige Mapping uebergeben.
// Aendert das Mapping <details> direkt.
private void _remove_details(string|string* keys, mapping details )
{
  if (stringp(keys))
    details -= ([keys]);
  else if (pointerp(keys))
    details -= mkmapping(keys);
  else
    raise_error("Wrong type to argument 1, expected string|string*.\n");
}

// Detail(s) hinzufuegen
void AddDetail(string|string* keys, string|string*|mapping|closure descr)
{
    int i;
    mapping details;

    details = Query(P_DETAILS, F_VALUE);

    // _add_details() aendern das Mapping direkt, Set etc. nicht noetig.
    return _add_details(keys, descr, details);
}

// Detail(s) entfernen
varargs void RemoveDetail(string|string* keys )
{
  // Alle loeschen geht direkt ...
  if (!keys )
    Set(P_DETAILS, ([]), F_VALUE);
  else
    // _remove_details() aendern das Mapping direkt, Set etc. nicht noetig.
    _remove_details(keys, Query(P_DETAILS, F_VALUE));
}

// SpecialDetail hinzufuegen
void AddSpecialDetail(string|string* keys, string functionname )
{
    closure cl;

    // Absichern! Sonst koennte jeder interne Funktionen aufrufen
    if (extern_call() &&
        (geteuid(previous_object()) != geteuid() || process_call()) &&
        !(object_name(previous_object()) == "/obj/doormaster" &&
          functionname == "special_detail_doors") )
      raise_error( "Illegal use of AddSpecialDetail!\n" );

    // Closure generieren
    if ( !stringp(functionname)||
         !(cl = symbol_function( functionname, this_object())) )
      return;

    // Detail hinzufuegen
    AddDetail( keys, cl );
    return;
}

// SpecialDetail(s) entfernen
void RemoveSpecialDetail(string|string* keys )
{
  // RemoveSpecialDetail(0) wuerde sonst ALLE Details (auch die
  // 'normalen') loeschen
  if (pointerp(keys)||stringp(keys))
    RemoveDetail(keys);
  return;
}

// Lesbares Detail einfuegen
void AddReadDetail(string|string* keys,
                   string|string*|mapping|closure descr )
{
  // _add_details() aendern das Mapping direkt, Set etc. nicht noetig.
  return _add_details(keys, descr, Query(P_READ_DETAILS, F_VALUE));
}

// Lesbare(s) Detail(s) entfernen
varargs void RemoveReadDetail(string|string* keys )
{
  // Alle loeschen geht direkt ...
  if (!keys )
    Set(P_READ_DETAILS, ([]), F_VALUE);
  else
    // _remove_details() aendern das Mapping direkt, Set etc. nicht noetig.
    _remove_details(keys, Query(P_READ_DETAILS, F_VALUE));
}

// Geraeusch(e) dazufuegen
void AddSounds(string|string* keys,
               string|string*|mapping|closure descr )
{
  // _add_details() aendern das Mapping direkt, Set etc. nicht noetig.
  return _add_details(keys, descr, Query(P_SOUNDS, F_VALUE));
}

// Geraeusch(e) entfernen
varargs void RemoveSounds(string|string* keys )
{
  // Alle loeschen geht direkt ...
  if (!keys )
    Set(P_SOUNDS, ([]), F_VALUE);
  else
    // _remove_details() aendern das Mapping direkt, Set etc. nicht noetig.
    _remove_details(keys, Query(P_SOUNDS, F_VALUE));
}

// Geru(e)ch(e) hinzufuegen
void AddSmells(string|string* keys,
               string|string*|mapping|closure descr )
{
  // _add_details() aendern das Mapping direkt, Set etc. nicht noetig.
  return _add_details(keys, descr, Query(P_SMELLS, F_VALUE));
}

// Geru(e)ch(e) entfernen
varargs void RemoveSmells(string|string* keys )
{
  // Alle loeschen geht direkt ...
  if (!keys )
    Set(P_SMELLS, ([]), F_VALUE);
  else
    // _remove_details() aendern das Mapping direkt, Set etc. nicht noetig.
    _remove_details(keys, Query(P_SMELLS, F_VALUE));
}

// Tastbare(s) Detail(s) hinzufuegen
void AddTouchDetail(string|string* keys,
                    string|string*|mapping|closure descr )
{
  // _add_details() aendern das Mapping direkt, Set etc. nicht noetig.
  return _add_details(keys, descr, Query(P_TOUCH_DETAILS, F_VALUE));
}

// Tastbare(s) Detail(s) entfernen
varargs void RemoveTouchDetails(string|string* keys )
{
  // Alle loeschen geht direkt ...
  if (!keys )
    Set(P_TOUCH_DETAILS, ([]), F_VALUE);
  else
    // _remove_details() aendern das Mapping direkt, Set etc. nicht noetig.
    _remove_details(keys, Query(P_TOUCH_DETAILS, F_VALUE));
}

// Detailinfos fuer Detail key, Spieler hat die Rasse race
// und benutzt seinen Sinn sense
varargs string GetDetail(string key, string race, int sense)
{
  string|string*|mapping|closure detail;
  
  if (stringp(race)) race = lower_case(race);
  
  switch(sense)
  {
    case SENSE_SMELL: detail=Query(P_SMELLS, F_VALUE)[key];
                      sense=EP_SMELL; break;
    case SENSE_SOUND: detail=Query(P_SOUNDS, F_VALUE)[key];
                      sense=EP_SOUND; break;
    case SENSE_TOUCH: detail=Query(P_TOUCH_DETAILS, F_VALUE)[key];
                      sense=EP_TOUCH; break;
    case SENSE_READ:  detail=Query(P_READ_DETAILS, F_VALUE)[key];
                      sense=EP_RDET;
                      break;

    default:          detail=Query(P_DETAILS, F_VALUE)[key];
                      sense=EP_DETAIL; break;
  }

  if (!stringp(detail))
  {
    if (closurep(detail))
      detail = (string)funcall(detail,key);
    else if (mappingp(detail))
      detail = (string)(detail[race]||detail[0]);
    else if (pointerp(detail))
      detail = (string)(detail[random(sizeof(detail))]);
  }

  // FP vergeben (so vorhanden ;-) )
  if (detail) GiveEP(sense,key);

  return detail;
}

// TODO: OBSOLET (Libgrep notwendig)
void read( string str ) {
  raise_error("Diese Funktion existiert nicht mehr.\n");
}


//                      ######################
//####################### Zugriffsfunktionen ############################
//                      ######################

// Dienen dazu, die direkte Manipulation der Props von aussen zu erschweren.

// Filter, um Specialdetails zu eliminieren
// erstellt ausserdem ne Kopie vom Mapping. (Wichtig!)
// Wird vor allem benoetigt, um P_DETAILS in P_DETAILS und 
// P_SPECIAL_DETAILS zu treffen.
private int _closures(string x, mapping details, int yes ) 
{ 
    return yes ? closurep(details[x]) : !closurep(details[x]); 
}

static mapping _query_details()
{
  return filter_indices(Query(P_DETAILS, F_VALUE), #'_closures,
      Query(P_DETAILS, F_VALUE),0);
}

static mapping _query_special_details()
{
  return filter_indices(Query(P_DETAILS, F_VALUE),#'_closures,
      Query(P_DETAILS, F_VALUE),1);
}

static mapping _query_read_details() {
  return deep_copy(Query(P_READ_DETAILS, F_VALUE));
}

static mapping _query_sound_details() {
  return deep_copy(Query(P_SOUNDS, F_VALUE));
}

static mapping _query_smell_details() {
  return deep_copy(Query(P_SMELLS, F_VALUE));
}


//                    ##########################
//##################### Klassen-Mitgliedschaft ##########################
//                    ##########################

// Klasse hinzufuegen
public void AddClass(string|string* str)
{
  if (stringp(str)) 
      str = ({ str });
  // Aliase aufloesen und implizite Klassen addieren.
  str = (string*)CLASSDB->AddImplicitClasses(str);
  // Summe mit alten Klassen bilden und Doppelte eliminieren
  str = str + Query(P_CLASS, F_VALUE);
  Set( P_CLASS, m_indices(mkmapping(str)), F_VALUE);

  return;
}

// Klasse entfernen
void RemoveClass(string|string* str)
{
 if (stringp(str)) 
      str = ({ str });

  // Aliase aufloesen und implizite Klassen addieren.
  str = (string*)CLASSDB->AddImplicitClasses(str);

  // Und alle - inklusive impliziter Klassen - entfernen
  // TODO: Pruefen, ob dies die richtige Entscheidung ist.
  Set( P_CLASS, Query(P_CLASS, F_VALUE)-str, F_VALUE);

  return;
}

// Ist das Objekt Mitglied der Klasse str?
int is_class_member(string|string* str)
{
  // Keine Klasse, keine Mitgliedschaft ...
  if (!str || str=="") 
      return 0;

  // Es sollte schon ein Array sein
  if (stringp(str)) 
      str = ({ str });

  // Klassen und Ids ins Array
  string *classes=QueryProp(P_CLASS);
  if (!pointerp(classes))
    return 0;

  // .. und testen
  foreach(string class : str)
    if (member(classes,class) > -1 ) return 1;

  return 0;
}

// Klasse direkt setzen abfangen
static string* _set_class(string* classes )
{
  Set( P_CLASS, ({}), F_VALUE );
  AddClass(classes);
  return QueryProp(P_CLASS);
}

//                       #####################
//######################## Material-Handling ############################
//                       #####################

// Material setzen
static mapping _set_material(mapping|string|string* mat )
{
  mapping mats = ([]);
  
  if (mappingp(mat)) 
  {
    if( !sizeof(mat) || !widthof(mat) )
      raise_error(sprintf("P_MATERIAL: expected mapping with at least one "
        "key and one value, got %.50O\n",mat));
    else 
      mats = mat;
  }
  else if (stringp(mat))
    mats[mat]=100;
  else
  {
    int sz = sizeof(mat);
    // Kommt dann vor, wenn <mat> 0 oder ({}) ist.
    if ( !sz )
      raise_error(sprintf("P_MATERIAL: expected string or non-empty "
        "mapping|string*, got %.50O.\n", mat));
    mats = mkmapping(mat, allocate(sz, 100/sz));
  } 
  return Set( P_MATERIAL, mats, F_VALUE );
}

// Woraus besteht das Objekt?
static mapping _query_material()
{
  mixed res;
  
  if ( !mappingp(res = Query(P_MATERIAL, F_VALUE)) )
    return ([MAT_MISC:100]);
  
  return res;
}

// Anteil von mat am Objekt?
int QueryMaterial( string mat )
{
  mapping mats;
  
  if ( !mappingp(mats = QueryProp(P_MATERIAL)) )
    return 0;
  
  return mats[mat];
}

// Anteil der Gruppe am Objekt
int QueryMaterialGroup( string matgroup )
{
  return (int)call_other( MATERIALDB, "MaterialGroup",
                          QueryProp(P_MATERIAL), matgroup );
}


string MaterialList( int casus, mixed idinf )
{
  return (string)call_other( MATERIALDB, "ConvMaterialList",
                             QueryProp(P_MATERIAL), casus, idinf );
}

static int _set_size(int sz) {
//Groesse muss > 0 sein, alles andere ist unsinnig! (0 und neg. Groessen
//haben keine phys. Relevanz und machen u.U. Probleme mit Objekten, die
//Schaden in Abhaengigkeit der Groesse machen)
  if (sz>0)
    Set(P_SIZE,sz,F_VALUE);
  return(Query(P_SIZE,F_VALUE));
}

// P_CLONE_TIME
static int _query_clone_time() { return object_time(); }

