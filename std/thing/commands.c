// MorgenGrauen MUDlib
//
// thing/commands.c -- thing description
//
// $Id: commands.c 9514 2016-02-23 20:33:09Z Gloinson $
//
// Aus Regenbogen MUDLib
// aus Gueldenland/Wunderland MUDlib (Morgengrauen MUDlib)
//
// P_COMMANDS data-structure:
//
// AddCmd(verb,fun1,1);
// AddCmd(verb+syn1a|syn1b&syn2a|syn2b|syn2c,fun2,
//        error1_notify|error2_notify^error2_write);
// -->
// ([verb:
//    ({fun1,fun2});                                        // funs
//    ({1,({error1_notify, error2_write^error2_say, 1})});  // flags
//    ({0,({({syn1a,syn1b}),({syn2a,syn2b,syn2c})})});      // rules
//    0;                                                    // IDs
//    ({closure auf fun1, closure auf fun2}) ])             // Cache
//
// Funs:  ({<fun1> ,...
//          'fun' kann sein: Closure
//                           String: Methodenname - wird etwas geprueft
//                           Objekt: wenn keine Methode, this_object() fuer
//                                   intern, previous_object() fuer extern
//                           0 (erloschenes externes Objekt)
// Rules: ({<Regelsatz fuer fun1>, ({<1. Synonymgruppe>,
//                                   <2. Synonymgruppe, ...}), ...})
// Flags: ({<Flag fuer fun1>, ({<Fehlermeldung 1. Synonymgruppe>, ... ,
//                              [, Index fuer write anstatt notify_fail]}),
//            ... })
// IDs:   0 oder ({<ID fuer fun1>}) oder ({0, <ID fuer fun2>}) ...
// Cache: ({<closure fuer fun1>, ...
//        Cache-Closures sind bei Export (QueryProp) immer genullt
#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#include <moving.h>
#include <thing/language.h>
#include <exploration.h>
#include <defines.h>
#include <living/comm.h>
#include <functionlist.h>

#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <thing/description.h>
#include <thing/commands.h>
#undef NEED_PROTOTYPES

#define CMDS_WIDTH 5
// Mapping-Indizes
#define CMDIDX_FUN   0
#define CMDIDX_FLAG  1
#define CMDIDX_RULE  2
#define CMDIDX_ID    3
#define CMDIDX_CACHE 4

#ifdef DBG
#undef DBG
#endif
#define DBG(x) printf("Object %O tmpstr=%s\n", explode(object_name(this_object()),"#")[1], x);

private nosave mapping added_cmds;

protected int _cmd_syntaxhelp(string str, mixed *args)
{
  mapping|closure restr;
  mixed help = QueryProp(P_SYNTAX_HELP);
  if (pointerp(help))
  {
    restr = help[1];
    help = help[0];
  }
  // Restriktionen vor dem Anzeigen pruefen.
  if (mappingp(restr))
  {
    string res = "/std/restriction_checker"->check_restrictions(PL,restr);
    if (res)
    {
      PL->ReceiveMsg("Fuer " + name(WEN,1) + " darfst Du "
                     "die Syntaxhilfe (noch) nicht lesen:\n"
                     + res,
                     MT_NOTIFICATION|MSG_BS_LEAVE_LFS,
                     "syntaxhilfe",0,this_object());
      return 1;
    }
  }
  else if (closurep(restr))
  {
    string res = funcall(restr, ME);
    if (res)
    {
      if (intp(res))
        PL->ReceiveMsg("Fuer " + name(WEN,1) + " darfst Du "
                       "die Syntaxhilfe (noch) nicht lesen.",
                       MT_NOTIFICATION|MSG_BS_LEAVE_LFS,
                       "syntaxhilfe",0,this_object());
      else if (stringp(res))
        PL->ReceiveMsg(res,
                       MT_NOTIFICATION|MSG_BS_LEAVE_LFS,
                       "syntaxhilfe",0,this_object());
      return 1;
    }
  }

  if (stringp(help))
  {
    help = "Fuer " + name(WEN,1) + " gibt es folgende Syntaxhilfe:\n"
           + help;
  }
  else if (closurep(help))
  {
    help = funcall(help, this_object());
  }
  else
  {
    // wenn das Objekt keine Syntaxhilfe hat, braucht es das Kommando auch
    // nicht.
    notify_fail("Fuer " + name(WEN,1)
                + " gibt es keine Syntaxhilfe.\n");
    RemoveCmd(0,0, "_cmd_syntaxhelp");
    return 0;
  }
  if (stringp(help) && sizeof(help))
    PL->ReceiveMsg(help, MT_NOTIFICATION|MSG_BS_LEAVE_LFS,
                   "syntaxhilfe",0,this_object());

  return 1;
}

protected void create()
{
  AddCmd("syntaxhilfe&@ID", #'_cmd_syntaxhelp,
         "Fuer WAS moechtest Du eine Syntaxhilfe?\n",
         "_cmd_syntaxhelp");
}

protected void create_super() {
  set_next_reset(-1);
}

private closure _check_stringmethod(mixed func, int ex, string resp) {
  closure cl = symbol_function(func, this_object());
  if(!cl)
  {
    catch(raise_error(sprintf(
      resp+"string-Methode '%s' fehlt oder ist private.\n", func));
      publish);
    return 0;
  }

  // extern erstellte auf Sichtbarkeit pruefen und abbrechen/davor warnen
  if(ex) {
    mixed *fl = functionlist(this_object(), RETURN_FUNCTION_NAME|
                                            RETURN_FUNCTION_FLAGS);
    int index = member(fl, func)+1;
    if(index<=0 || sizeof(fl)<=index) // sollte nicht passieren, s.o.
      raise_error(sprintf(
        resp+"string-Methode '%s' trotz function_exists() nicht in "
             "functionlist. WTF?\n", func));
    else if(fl[index]&TYPE_MOD_PROTECTED || fl[index]&TYPE_MOD_STATIC) {
      catch(raise_error(sprintf(
        resp+"string-Methode '%s' ist protected/static. Extern "
        "definiertes Kommando darf so eine Methode nicht aufrufen!\n",
        func));
        publish);
      return 0;
    }
  }

  // ansonsten Methode erstmal cachen
  return cl;
}

varargs void AddCmd(mixed cmd, mixed func, mixed flag, mixed cmdid) {
 int i,j;
 closure cachedcl;
 mixed *rule;

 // potentielle AddCmd mit Regel?
 if(stringp(cmd)) {
  // eine Regel? - aufsplitten
  if((i=member(cmd,'&'))>0) {
   // ... in Array mit Verknuepfungselementen
   rule=explode(cmd[(i+1)..],"&");
   j=sizeof(rule);
   // ... in Array mit Arrays mit Alternativelementen:
   // "p|q&r|s" -> ({ ({"p","q"}), ({"r","s"}} })
   while(j--)
    rule[j]=explode(rule[j],"|");

   // Regeln von Kommandoverben abschneiden
   cmd=cmd[0..(i-1)];
  }
  // Kommandoverben extrahieren
  cmd=explode(cmd,"|");

  // Satz von Regeln existiert: Aufsplitten von Fehlermeldungen
  if(rule)
   if(stringp(flag)) {
    mixed *fail;
    // in einfaches Array mit jeweiligen Fehlermeldungen
    fail=explode(flag,"|");
    j=0;
    i=sizeof(fail);
    while(j<i) {
     // write - Fehlermeldung entdeckt - Position ggf. eintragen
     if(member(fail[j],'^')>=0 && !intp(fail[<1]))
      fail+=({j});
     if(member(fail[j],'@')>=0) {
      int s;
      flag=regexplode(fail[j], "@WE[A-SU]*[0-9]");
      s=sizeof(flag);
      while((s-=2)>0) {
       int tmpint;
       tmpint=flag[s][<1]-'1';
       if(tmpint<0 || tmpint>j)
        raise_error(sprintf(
         "AddCmd: error-message %d contains out-of-bounds @WExx-rule.\n",j+1));
      }
     }
     j++;
    }
    // "Was?|Wie das?" -> ({"Was?","Wie das?"})
    // "Was?|Wie das?^|Womit das?|Worauf das?^@WER1 macht was." ->
    //  ({"Was?",
    //    "Wie das?^Womit das?",
    //	  "Worauf das?^@WER1 macht was.",1})
    flag=sizeof(fail);
    if(flag && flag<sizeof(rule))
     raise_error(
      "AddCmd: number of error-messages does not match number of rules.\n");
    flag=fail; // ueberschreiben mit den parsefreundlichen Format
  } else if(flag)
   raise_error("AddCmd: rules exist but flags are not an error-string.\n");
 } // end if(stringp(cmd)) ... kein Regelstring vorhanden

 // kein Kommandoarray gewesen noch erzeugt?
 if(!pointerp(cmd))
   raise_error("AddCmd: missing string/pointer-parameter for command.\n");

  // String-Methode auf Sichtbarkeit pruefen, Cache gleich anpassen
  switch(typeof(func)) {
    case T_STRING:
      cachedcl=_check_stringmethod(func, extern_call(), "AddCmd: ");
      break;
    case T_CLOSURE:
      cachedcl=func;
      // an und fuer sich koennte man LFun-Stringnamen hier extrahieren,
      // um Serialisierung via P_COMMANDS zu ermoeglichen. Momentan: nein.
      func=0;
      break;
    default:
      if(extern_call()) func=previous_object();
      else func=this_object();
      break;
  }

  // jedes einzelne Verb mit seinen Regeln und Funktionen eintragen
 i=sizeof(cmd);
 if(!added_cmds) added_cmds=m_allocate(i, CMDS_WIDTH);
 while(i--) {
  string str;
  str=cmd[i];
  if(!member(added_cmds,str))
   added_cmds+=([str:allocate(0);allocate(0);allocate(0);0;allocate(0)]);
  // existierendes Verb ergaenzen
  added_cmds[str, CMDIDX_FUN]+=({func});
  added_cmds[str, CMDIDX_FLAG]+=({flag});
  added_cmds[str, CMDIDX_RULE]+=({rule});
  added_cmds[str, CMDIDX_CACHE]+=({cachedcl});
  // ggf. id in das ID-Mapping eintragen
  if(cmdid) {
   mixed *tmp;
   j=sizeof((string*)added_cmds[str, CMDIDX_FUN]);
   tmp=added_cmds[str, CMDIDX_ID]||allocate(j);
   if(sizeof(tmp)<j) tmp+=allocate(j-sizeof(tmp));
   tmp[<1]=cmdid;
   added_cmds[str, CMDIDX_ID]=tmp;
  }
 }
}

// Auswertung fuer ein Verb loeschen
varargs int RemoveCmd(mixed cmd, int del_norule, mixed onlyid) {
 int ret;

 // Falls Magier das RemoveCmd falsch nutzen (z.B. analog zu AddCmd)
 // wird das Regelsystem verwirrt. Da das del_norule nur int sein darf,
 // gibt es hier eine gute Chance den Fehler abwaertskompatibel zu ent-
 // decken. Damit Spieler den Fehler nicht mitbekommen, wird hier auf
 // ein raise_error verzichtet, und statt dessen in ein Logfile ge-
 // schrieben.
 if (!intp(del_norule))
 {
   log_file("REMOVE_CMD",
     sprintf("\n-- %s --\nIllegal RemoveCommand() in Object [%O]:\n %O\n",
       dtime(time()), this_object(), cmd));
   del_norule=0;
   onlyid=0;
 }
 
 if(!added_cmds || (!cmd && !del_norule && !onlyid))
  added_cmds=m_allocate(0, CMDS_WIDTH); 
 else {
  int i, j;
  mixed *rule, *flag, *fun, *delrule, *ids, *cachecl;

  if(stringp(cmd)) {
   // Regeln entdeckt - Zerlegen (wie AddCmd)
   if((i=member(cmd,'&'))>0) {
    delrule=explode(cmd[(i+1)..],"&");
    j=sizeof(delrule);
    while(j--)
     delrule[j]=explode(delrule[j],"|");
    cmd=cmd[0..(i-1)];
   }
   cmd=explode(cmd,"|");
  } else if(del_norule || onlyid) cmd=m_indices(added_cmds);

  if(!pointerp(cmd))
   raise_error("RemoveCmd: missing string/pointer-parameter.\n");
  i=sizeof(cmd);

  while(i--) {
   // keine Regeln da und Regeln loeschen erlaubt: alles loeschen
   if(!delrule && !del_norule && !onlyid) m_delete(added_cmds,cmd[i]);
   else if(m_contains(&fun, &flag, &rule, &ids, &cachecl, added_cmds, cmd[i])) {
    j=sizeof(fun);
    while(j--) {
     int k;
     // DBG(rule[j]);
    	// Regeln nicht loeschen und Regel?
     if(!(del_norule && pointerp(rule[j])) &&
        // nur bestimmte ID loeschen und ID passt nicht?
        !(onlyid && (!pointerp(ids) || sizeof(ids)<=j || ids[j]!=onlyid)) &&
        // Loeschregel existiert und passt nicht auf Regel?
        !(delrule && (k=sizeof(rule[j]))!=sizeof(delrule))) {
      // partielles Testen einer Loeschregel ...
      if(delrule) {
       while(k--)
        if(!sizeof(rule[j][k]&delrule[k])) break;
       if(k>=0) continue;
      }
      // alles korrekt: Loeschen!
      // (Arraybereich durch leeres Array loeschen)
      flag[j..j]    = allocate(0);
      fun[j..j]     = allocate(0);
      rule[j..j]    = allocate(0);
      cachecl[j..j] = allocate(0);
      if(ids) {
       ids[j..j] = allocate(0);
       if(!sizeof(ids-allocate(1))) ids=(mixed*)0;
      }
      ret++;
     }
    } // end while(j--) {
   }
   // Funktions/Regelliste update oder ggf. Kommando voellig loeschen
   if(sizeof(rule)) {
    added_cmds[cmd[i], CMDIDX_FUN]=fun;
    added_cmds[cmd[i], CMDIDX_FLAG]=flag;
    added_cmds[cmd[i], CMDIDX_RULE]=rule;
    added_cmds[cmd[i], CMDIDX_ID]=ids;
    added_cmds[cmd[i], CMDIDX_CACHE]=cachecl;
   } else m_delete(added_cmds,cmd[i]);
  }
 }
 return ret;
}

// Ausfuehren samt geparstem Inputstring und getriggerten Parserergebnissen
static int _execute(mixed fun, string str, mixed *parsed) {
  switch(typeof(fun)) {
    case T_CLOSURE:
      return ((int)funcall(fun,str,&parsed));
    case T_STRING:
      int ret;
      if(!call_resolved(&ret, this_object(), fun, str, &parsed))
        raise_error(sprintf(
          "AddCmd: String-Methode '%s' in Kommando %#O scheint zu fehlen "
          "oder nicht zugreifbar zu sein.\n", fun, parsed));
      return ret;
    default:
      break;
  }
  return 0;
}

#define CHECK_PRESENT     1
#define CHECK_ID          2
#define CHECK_PUTGETNONE  4
#define CHECK_PUTGETDROP  8
#define CHECK_PUTGETTAKE  16
#define CHECK_PUTGET      (CHECK_PUTGETNONE|CHECK_PUTGETDROP|CHECK_PUTGETTAKE)

// Wert fuer Fehlschlag, Fallback-Wert der benutzten while-- - Schleifen
#define NOMATCHFOUND      -1

// Regeln fuer ein (nun unwichtiges) Verb triggern
static int _process_command(string str, string *noparsestr,
                            mixed fun, mixed flag, mixed rule,
                            mixed id, mixed cachedcl) {
 mixed *parsed, *objmatches;

 // eine Regel ... auswerten ...
 if(pointerp(rule)) {
  int nrul;
  parsed=objmatches=allocate(0);
  int lastmatchpos=NOMATCHFOUND;

  // Abgleichen der gesplitteten Eingabe mit Regeln:
  // vorwaerts durch die Synonymgruppen
  int rs=sizeof(rule);
  while(nrul<rs) {
   int matchpos;
   string *synonym;
   mixed matchstr;

   matchpos=NOMATCHFOUND;
   matchstr=0;

   // Synonyme extrahieren
   int nrsynonyms=sizeof(synonym=rule[nrul]);

   // egal wie durch Synonyme bis Match - Abgleich mit Eingabe
   while(nrsynonyms--) {
    int tmppos = member(noparsestr,synonym[nrsynonyms]);
    // ist Synonym im Eingabestring und kommt spaeter als vorheriges Synonym?
    if(tmppos>=0 && tmppos>lastmatchpos) {
     // Erfolg: merken der Position im Eingabestring und den matchenden String
     matchpos=tmppos;
     matchstr=noparsestr[tmppos];
     break;
    }
   }

   // kein Match durch Synonyme? Pruefe die @-Spezialvariablen.
   if(matchpos == NOMATCHFOUND) {
    int check_present;
    // ist Abpruefen von ID/PRESENT in der Synonymgruppe verlangt
    // bei present()/find_obs gleich Voraussetzung gueltiger TP mitprufen
    if(member(synonym,"@ID")>=0) check_present=CHECK_ID;
    if(this_player()) {
     if(member(synonym,"@PRESENT")>=0) check_present|=CHECK_PRESENT;
     else if(member(synonym,"@PUT_GET_NONE")>=0)
		check_present|=CHECK_PUTGETNONE;
     else if(member(synonym,"@PUT_GET_TAKE")>=0)
		check_present|=CHECK_PUTGETDROP;
     else if(member(synonym,"@PUT_GET_DROP")>=0)
		check_present|=CHECK_PUTGETTAKE;
    }

    if(check_present) {
     // wir fangen hinter dem letzten Match an
     int q_start=lastmatchpos+1;
     int r_end=sizeof(noparsestr)-1;

     int range;
     while((range=r_end-q_start)>=0) {
      mixed tmpobj;

      // wie weit wollen wir zurückgehen?
      if(range)
        if(!(check_present&CHECK_PUTGET))
          range=range>2?2:range; // 3 Fragmente fuer @ID/@PRESENT (Adverb/Nr)
        else if(range>4)
          range=4;               // 5 Fragmente fuer @PUT_XXX 

      // und jetzt die Substrings pruefen
      while(range>=0 && !matchstr) {
       string tmpstr;

       // zu pruefenden String aus den Teilen zusammensetzen
       if(range) tmpstr=implode(noparsestr[q_start..(q_start+range)]," ");
       else tmpstr=noparsestr[q_start];

       //DBG(tmpstr);
       if(check_present&CHECK_PRESENT &&			// PRESENT ?
          ((tmpobj=present(tmpstr,this_player())) ||
           (tmpobj=present(tmpstr,environment(this_player())))))
        matchstr=tmpobj;
       else if(check_present&CHECK_ID && id(tmpstr))	// ID ?
        matchstr=this_object();
       else if((check_present&CHECK_PUTGET) &&	// PUT_GET_??? ?
               (tmpobj=(object*)
                  this_player()->find_obs(tmpstr,
                      ([CHECK_PUTGETNONE:PUT_GET_NONE,
                        CHECK_PUTGETDROP:PUT_GET_TAKE,
                        CHECK_PUTGETTAKE:PUT_GET_DROP])
                                        [CHECK_PUTGET&check_present])) &&
               sizeof(tmpobj)) {
        if(sizeof(tmpobj)==1) matchstr=tmpobj[0];
        else {	// Arrays werden zwischengespeichert ...
         objmatches+=({sizeof(parsed),tmpobj});
         matchstr=tmpstr;
        }
       } else { // dieser Substring hat nicht gematcht
        // ab weniger als 3 Teilen ist das Nichtmatching eines Substrings mit
        // beendendem Numeral Kriterium fuer Abbruch ("objekt 2" soll matchen)
        string numeralcheck;
        if(range && range<=2 &&
           sizeof(numeralcheck=noparsestr[q_start+range]) &&
           to_string(to_int(numeralcheck))==numeralcheck)
          break;

        // Substringlaenge verkuerzen und weiter
        range--;
       }
      }

      // Match gefunden!
      if(matchstr) {
       matchpos=range+q_start;
       // DBG(matchpos);
       break;
      }
      q_start++;
     } // end while
    }
   }

   // Fehlermeldung fuer diese fehlgeschlagene Synonymgruppe setzen
   if(matchpos == NOMATCHFOUND) {
    // Fehlermeldungen und ein Eintrag an der Fehlerstelle?
    if(pointerp(flag) && sizeof(flag)>nrul) {

     matchstr=flag[nrul];

     if(stringp(matchstr) && sizeof(matchstr)) {
      if(member(matchstr,'@')>=0) {
       matchstr=replace_personal(&matchstr,({this_player()})+parsed,1);
       string stamm=((query_verb()[<1]^'e')?query_verb():query_verb()[0..<2]);
       matchstr=regreplace(matchstr,"@VERB",capitalize(stamm),1);
       matchstr=regreplace(matchstr,"@verb",stamm,1);
      }

      // ist Fehlermeldung ein WRITE?
      // dann return 1 !
      if(intp(flag[<1]) && flag[<1]<=nrul) {
       if(member(matchstr,'^')>=0) {
        matchstr=explode(matchstr,"^");
        write(capitalize(break_string(matchstr[0],78,0,1)));
        if(sizeof(matchstr[1]))
         say(capitalize(break_string(matchstr[1],78,0,1)),({this_player()}) );
       } else write(capitalize(break_string(matchstr,78,0,1)));
       return 1;
      } else notify_fail(capitalize(break_string(matchstr,78,0,1)));
     }
    }
    return 0;
   }

   // Updaten der Hilfsvariablen
   parsed+=({matchstr});
   lastmatchpos=matchpos;
   nrul++;
  } // end while(nrul<rs) ... Erfolg ... ab zum naechsten Regelteil
 }

 // Arrays der @-Objectmatches in Parameter fuer Methode resubstituieren
 int objsize;
 if((objsize=sizeof(objmatches)))
  while((objsize-=2)>=0)
   parsed[objmatches[objsize]]=objmatches[objsize+1];

 // erfolgreich Methode gefunden/eine Regel bis zum Ende durchgeparst:
 return(_execute(cachedcl||fun, str, &parsed));
}

// Auswertung - add_action-Fun 
public int _cl(string str) {
 int nindex;
 string *keys;
 mixed *flag;
 // Verb existiert, Kommandos auch, Eintrag fuer Verb gefunden
 if(mappingp(added_cmds) && query_verb()) {
  mixed *fun, *rule, *ids, *cachecl;

  // ist das Verb ein Key im Kommandomapping?
  if(m_contains(&fun, &flag, &rule, &ids, &cachecl, added_cmds, query_verb())) {
   string *noparsestr;
   nindex=sizeof(fun);
   noparsestr=explode((str||"")," ")-({""});
   // dann matche ungeparsten Input gegen etwaige Regeln
   // -- nicht aendern: neue Kommandos sollen alte "ueberschreiben" koennen
   while(nindex--)
   {
    mixed cmd_id = (pointerp(ids) && sizeof(ids)>nindex) ? ids[nindex] : 0;
    if(_process_command(&str, noparsestr, fun[nindex], flag[nindex],
                        rule[nindex], cmd_id, cachecl[nindex])) {
      GiveEP(EP_CMD, query_verb());
      return 1;
    }
   }
  }

  // keine Regel passte, unscharfe Auswertung auf alle
  // AddCmdverben ausdehnen
  keys=m_indices(added_cmds);
  nindex=sizeof(keys);
  while(nindex--)
   if(!strstr(query_verb(),keys[nindex]) &&
      member((flag=added_cmds[keys[nindex], CMDIDX_FLAG]),1)>=0) {
    int i,ret;
    i=sizeof(flag);
    // Reihenfolge nicht aendern !
    while(i--)
      if(flag[i]==1) {
        mixed *fuzzyfuns   = added_cmds[keys[nindex], CMDIDX_FUN];
        mixed *fuzzycache  = added_cmds[keys[nindex], CMDIDX_CACHE];
        if(!pointerp(fuzzyfuns) || sizeof(fuzzyfuns)<=i)
          catch(raise_error(
            "AddCmd-Auswertung: CatchAll-Kommando \""+keys[nindex]+"\""
            " wurde waehrend der Ausfuehrung veraendert oder geloescht. "
            "Klartext: Das ist schlecht. Sucht ein RemoveCmd? ");
            publish);
        else if(_execute(fuzzycache[i]||fuzzyfuns[i], str, 0)) {
          GiveEP(EP_CMD, query_verb());
          return 1;
        }
      }
   }
 }
 return 0;
}

public varargs void init(object origin) {
 add_action("_cl","",1);
}


private void _check_importentry(string ind, mixed *fun, mixed *flag,
                                mixed *rule, int|mixed id, mixed *cachedcl) {
  // Check der Stringmethoden: Fehler werden abgefangen, um generelles
  // Kopieren zu ermoeglichen. Es koennen aber Kommandomethoden verloren gehen.
  cachedcl = map(fun, function int|closure(string clfun) {
                        closure ret;
                        catch(ret=_check_stringmethod(
                                    clfun, 1,
                                    "Warnung SetProp(P_COMMANDS): ");
                              publish);
                        return ret;
                      });
  if(sizeof(fun)!=sizeof(flag) || sizeof(fun)!=sizeof(rule))
    raise_error("SetProp(P_COMMANDS): corrupt commands-mapping.\n");
}

private void _cleanup_exportcl(string ind, mixed *fun, mixed *flag,
                               mixed *rule, int|mixed id,
                               mixed *cachedcl) {
  cachedcl = allocate(sizeof(fun));
}

static mapping _query_commands() {
  mapping ret;
  if(mappingp(added_cmds)) {
    ret = deep_copy(added_cmds);
    // Closures im Cache werden immer geloescht, da sonst versehentlich
    // doch exportiert werden koennte
    walk_mapping(ret, #'_cleanup_exportcl);
  }
  return ret;
}

static mapping _set_commands(mapping commands) {
  if(!commands) added_cmds=(mapping)0;
  else if(mappingp(commands)) {
    if(widthof(commands) != CMDS_WIDTH)
      raise_error("SetProp(P_COMMANDS): corrupt commands-mapping.\n");
    mapping tmp = deep_copy(commands);
    walk_mapping(tmp, #'_check_importentry);
    added_cmds = tmp;
  }
  return _query_commands();
}
