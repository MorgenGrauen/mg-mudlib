// MorgenGrauen MUDlib
//
// npc/info.c -- Behandeln von Fragen an den NPC
//
// $Id: info.c 9522 2016-03-01 19:20:10Z Arathorn $

/* Letzte Aenderungen von Wim 8.1.99
 *
 * AddInfo( schluessel, antwort [, indent [, [silent [, casebased] ] ] )
 *  Wenn ein Spieler dieses Monster nach "schluessel" fragt, so gib die
 *  Programmierte Antwort aus.
 *  Erweiterung von Wim: ist silent gesetzt, so erfolgt eine "persoenliche"
 *    Antwort. d.h. umstehende Personen bekommen bei silent==1 keinen, bei
 *    stringp(silent), den String ausgegeben, dabei kann er auch die Schluessel-
 *    Worte @WER @WESSEN @WEM @WEN enthalten.
 *  - Ergaenzt um @CAP_WER... fuer zwangs-capitalisierte Namen an Satzanfaengen.
 *    ist bei fragenden NPCs und PCs mit Tarnkappe wichtig! (Silvana)
 *  - Auch in der Antwort des fragenden wird nun ersetzt (Vanion)
 *  Enthaelt casedbased einen Funktionsnamen oder verweist auf eine closure, so
 *    wird die Beantwortung der Frage von dem return-Wert abhaengig gemacht.
 *    Bei 0 wird die Frage normal beantwortet, bei 1 erfolgt die Ausgabe des
 *    unter DEFAULT_NOINFO gespeicherten Textes.
 *    Wird ein String zurueckgegeben, so wird er unter Beachtung von ident an
 *    Stelle der urspruenglichen Information ausgegeben.
 *
 * RemoveInfo( schluessel )
 *  Das Monster antwortet nicht mehr auf diesen Schluessel.
 *
 * SetProp( P_DEFAULT_INFO, antwort [, indent ] )
 *  Setze die Antwort, die das Monster auf unverstaendliche Fragen geben
 *  soll. (Diese Funktion ist obsolet! Benutze stattdessen
 *  AddInfo( "\ndefault info", antwort [, indent ] );
 *
 * GetInfo( [schluessel] )
 *  Wenn Schluessel gesetzt ist, so wird die dazugehoerige Info,
 *  ansonsten werden alle Infos zurueckgegeben.
 *
 * Die Antworten sollten wie emote - kommandos aussehen.
 * Der optionale Indent wird zum Umbrechen von langen Infos benutzt.
 * (Typischerweise sollte indent="sagt: " sein.)
 *
 * In den Infos darf mit process_string gearbeitet werden. Das Ergebnis von
 * process_string wird dann mit umgebrochen!
 *
 *---------------------------------------------------------------------------
 */
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone

#define NEED_PROTOTYPES
#include <thing/description.h>
#include <thing/properties.h>
#include <npc.h>
#undef NEED_PROTOTYPES

#include <properties.h>
#include <language.h>
#include <defines.h>
#include <config.h>
#include <exploration.h>

// TODO: langfristig waer hier private schoen.
nosave mapping infos;

protected void create()
{
    // Initialisierung nur wenn noetig, damit beim virtuellen Erben von
    // /std/npc in npc1 und npc2 dann in npc3 beim npc1::create();
    // npc2:create(); im zweiten create() die Infos nicht
    // ueberschrieben/geloescht werden.
    if (!mappingp(infos))
    {
        infos = m_allocate(20,4);
        AddInfo(DEFAULT_INFO, "schaut Dich fragend an.\n", 0,
                              "schaut @WEN1 fragend an.\n", 0);
        AddInfo(DEFAULT_NOINFO, "moechte Dir nicht antworten.\n", 0,
                                "verweigert @WEM1 die Antwort.\n",
                                function int () { return 1; });
    }
}


public varargs void init(object origin)
{
  add_action( "frage", "frag", 1 );
}


static void smart_npc_log(string str)
{
  string creat_det;
  string|int creat = QueryProp(P_LOG_INFO);
  if (!stringp(creat)) {
    creat = MASTER->creator_file(this_object());
    if (creat == ROOTID)
      creat = "ROOT";
    else if( creat==BACKBONEID )
      creat="STD";
    creat_det="report/"+explode(creat, ".")[<1]+"_INFO.rep";
    creat="report/"+explode(creat, ".")[<1]+".rep";
  }
  log_file(creat,
           sprintf("INFO von %s [%s] (%s):\n%s\n",
                   getuid(this_interactive()),
                   explode(object_name(this_object()),"#")[0],
                   strftime("%d. %b %Y"),
                   str));
  if (stringp(creat_det) && sizeof(creat_det))
    log_file(creat_det,
             sprintf("INFO von %s [%s] (%s):\n%s\n",
                     getuid(this_interactive()),
                     explode(object_name(this_object()),"#")[0],
                     strftime("%d. %b %Y"),
                     str));
}

public int frage(string str) {
  string myname, text;

  str=(extern_call()?this_player()->_unparsed_args():str);
  if( !str || sscanf( str, "%s nach %s", myname, text ) != 2 ) {
    _notify_fail( "WEN willst Du nach WAS fragen?\n" );
    return 0;
  }

  if( !id( lower_case(myname) )
    || QueryProp(P_INVIS) ) {
    _notify_fail( "So jemanden findest Du hier nicht.\n" );
    return 0;
  }

  if (this_player()->QueryProp(P_DEAF))
  {
    this_player()->ReceiveMsg(
      "Warum jemanden etwas fragen, wenn Du die Antwort gar nicht hoeren "
      "koenntest?",
      MT_NOTIFICATION, "frage");
    return 1;
  }

  say( capitalize(this_player()->name(WER))+" fragt " +
    name(WEN,2)+" nach "+capitalize(text)+".\n",
      this_player() );

  text = lower_case(text);
  GiveEP(EP_INFO, text);

  return do_frage( text );
}

// ersetzt die alten @... durch die von replace_personal.
private string conv2replace_personal(string pstring)
{
  pstring=" "+pstring;
  if (strstr(pstring,"@WER",0) >-1 )
    pstring= regreplace(pstring, "@WER([^1-9QU])", "@WER1\\1", 1);
  if (strstr(pstring,"@WESSEN",0) >-1 )
    pstring= regreplace(pstring, "@WESSEN([^1-9QU])", "@WESSEN1\\1", 1);
  if (strstr(pstring,"@WEM",0) >-1 )
    pstring= regreplace(pstring, "@WEM([^1-9QU])", "@WEM1\\1", 1);
  if (strstr(pstring,"@WEN",0) >-1 )
    pstring= regreplace(pstring, "@WEN([^1-9QU])", "@WEN1\\1", 1);
  if (strstr(pstring,"@CAP_WER",0) >-1 )
    pstring= regreplace(pstring,"@CAP_WER","@WER1",1);
  if (strstr(pstring,"@CAP_WESSEN",0) >-1 )
    pstring= regreplace(pstring,"@CAP_WESSEN","@WESSEN1",1);
  if (strstr(pstring,"@CAP_WEM",0) >-1 )
    pstring= regreplace(pstring,"@CAP_WEM","@WEM1",1);
  if (strstr(pstring,"@CAP_WEN",0) >-1 )
    pstring= regreplace(pstring,"@CAP_WEN","@WEN1",1);

  return pstring[1..];
}

static mixed *GetInfoArr(string str)
{  
   return ({ infos[str, 0], infos[str, 1], infos[str,2], infos[str, 3] });
}

public int do_frage(string text)
{
  string indent,answer;
  mixed silent, preinfo, noanswer;
  mixed *info;

  if (stringp(preinfo = QueryProp(P_PRE_INFO)))
  {
     // Die message action wird auf "frage" fixiert, damit dies immer das
     // gleiche ist, egal, mit welchem Verb man in diese Funktion kommt.
     this_player()->ReceiveMsg(preinfo, MT_LISTEN, "frage",
                               Name(WER,2)+" ",this_object());
     send_room(environment(this_object()),
               "ist nicht gewillt, "+this_player()->Name(WEM,2)
               +" zu antworten.",
               MT_LISTEN, "frage",
               Name(WER,2)+" ",
               ({this_player()}) );
     return 1;
  }
  else
  {
    if (intp(preinfo) && preinfo > 0)
      return 1;
  }

  info=GetInfoArr(text);
  if (!info[0])
  {
    if( this_interactive() && QueryProp(P_LOG_INFO) )
      smart_npc_log(text);
    text = DEFAULT_INFO;
    info=GetInfoArr(text);
  }

  if (closurep(info[0]) ) {
    answer=funcall(info[0]);
    if( !answer || answer=="") return 1;
  } else {
    answer=process_string(info[0]);
  }

  if (closurep(info[3]) )
   {
    noanswer=funcall(info[3]);
    if ( intp(noanswer) && noanswer > 0)
     {
       text = DEFAULT_NOINFO;
       info = GetInfoArr(text);
       if (closurep(info[0]) ) {
         answer=funcall(info[0]);
         if( !answer || answer=="") return 1;
       } else {
         answer=process_string(info[0]);
       }
     }
    else if ( stringp(noanswer) )
      answer = noanswer;
   }

  silent=info[2];

  // Replacements gehen auch in der Antwort des NPC. Das gibt den Antworten 
  // eine persoenliche Note, und so teuer is das auch nicht :)
  answer = replace_personal(answer, ({this_player()}), 1);

  if( indent=info[1] )
  {
    if (stringp(silent) || (intp(silent) && silent > 0) )
    {  // Persoenliche Antwort mit indent
       this_player()->ReceiveMsg(answer, MT_LISTEN|MSG_BS_LEAVE_LFS,
                                 "frage",
                                 Name(WER,2)+" "+indent,
                                 this_object());
       if (stringp(silent))
       {
          silent=replace_personal(silent, ({this_player()}), 1);
          send_room(environment(), silent, MT_LISTEN, "frage",
                    Name(WER,2)+" ", ({this_player()}));
       }
    }
    else // "normale Antwort" mit Indent
    {
      send_room(environment(), answer, MT_LISTEN|MSG_BS_LEAVE_LFS,
                "frage",
                Name(WER,2)+" "+indent, ({this_object()}));
    }
  }
  else
  {
    if (stringp(silent) || (intp(silent) && silent > 0) )
    {  // Persoenliche Antwort ohne indent
       this_player()->ReceiveMsg(answer, MT_LISTEN|MSG_DONT_WRAP,
                                 "frage",
                                 Name(WER,2)+" ",
                                 this_object());
       if (stringp(silent))
       {
          silent=replace_personal(silent, ({this_player()}), 1);
          send_room(environment(), silent, MT_LISTEN, "frage",
                    Name(WER,2)+" ", ({this_player()}) );
       }
    }
    else // "normale Antwort" ohne Indent
      send_room(environment(), answer, MT_LISTEN|MSG_DONT_WRAP,
                "frage", Name(WER,2)+" ");
  }
  return 1;
}

/*
 *---------------------------------------------------------------------------
 * Setzen von Infos
 *---------------------------------------------------------------------------
 */

public varargs void AddInfo(string|string* key, string|closure info,
               string indent, int|string silent, string|closure casebased)
{

  if (stringp(casebased))
    casebased=symbol_function(casebased,this_object());
  if (stringp(info))
    info = conv2replace_personal(info);
  if (stringp(silent))
    silent = conv2replace_personal(silent);

  if( pointerp( key ) ) {
    int i;
    for ( i=sizeof( key )-1; i>=0; i-- )
      infos += ([ key[i]: info; indent; silent; casebased ]);
  }
  else
    infos += ([ key: info; indent; silent; casebased ]);
}

public varargs void AddSpecialInfo(string|string* key, string functionname,
               string indent, int|string silent, string|closure casebased)
{
  closure cl = symbol_function(functionname,this_object());

  if (!cl)
    return;
  return AddInfo(key, cl, indent, silent, casebased);
}


public void RemoveInfo( string key )
{
  m_delete(infos,key);
}

static varargs void _set_default_info( mixed info )
{
  if (pointerp(info))
    apply(#'AddInfo/*'*/,DEFAULT_INFO,info);
  else
    AddInfo(DEFAULT_INFO,info);
}

public varargs mixed GetInfo(string str)
{
  if (!str) return deep_copy(infos);
  return infos[str];
}


static mapping _query_npc_info()
{
    return deep_copy(infos);
}


static mapping _set_npc_info( mapping map_ldfied )
{
    if ( !mappingp(map_ldfied) )
        return 0;

    return infos = map_ldfied;
}

