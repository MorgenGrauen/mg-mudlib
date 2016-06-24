//
// pub.c -- Alles, was eine Kneipe braucht.
//
// $Id: pub.c 8778 2014-04-30 23:04:06Z Zesstra $
// spendiere ueberarbeitet, 22.05.2007 - Miril
#pragma strong_types
#pragma save_types
#pragma pedantic
#pragma range_check
#pragma no_clone

#define NEED_PROTOTYPES
#include <thing/commands.h>
#include <thing/properties.h>

#include <defines.h>
#include <rooms.h>
#include <properties.h>
#include <routingd.h>
#include <bank.h>
#include <exploration.h>
#include <wizlevels.h>
#include <pub.h>

// Alle nicht-privaten werden in erbenen Objekten verwendet.
private nosave int     max_list;
private nosave int     refresh_count;
private nosave int     sum;
private nosave mapping refresh_list;
nosave mapping id_list;
nosave mapping menu_list;
nosave object  *waiting;

#define PM_RATE_PUBMASTER  "rate"
#define PM_DELAY_PUBMASTER "delay"

protected void create()
{ object router;

  SetProp( P_ROOM_TYPE, QueryProp(P_ROOM_TYPE) | RT_PUB );

  SetProp( P_PUB_NOT_ON_MENU,
    "Tut mir leid, das fuehren wir nicht! Wir sind ein anstaendiges "+
    "Lokal!\n" );
  SetProp( P_PUB_UNAVAILABLE,
    "Davon ist leider nichts mehr da.\n" );
  SetProp(P_PUB_NO_MONEY,
    "Das kostet %d Goldstuecke, und Du hast nicht so viel!\n" );
  SetProp(P_PUB_NO_KEEPER,
    "Es ist niemand anwesend, der Dich bedienen koennte.\n");

  AddCmd( "menue","menue" );
  AddCmd( ({"kauf","kaufe","bestell","bestelle"}),"bestelle" );
  AddCmd( ({"spendier","spendiere"}),"spendiere" );
  AddCmd( "pubinit","pubinit" );

  max_list=0;
  refresh_count=0;
  waiting = ({ });
  id_list=([]);
  menu_list=([]);
  refresh_list=([]);

  if ( !clonep(ME) && objectp(router=find_object(ROUTER)) )
    router->RegisterTarget(TARGET_PUB,object_name(ME));

  call_out("add_std_drinks",1);
}

protected void create_super() {
  set_next_reset(-1);
}

/* Zur Syntax:
 *
 * menuetext - Der Text steht im Menue
 *
 * ids       - Array der Namen, mit denen bestellt werden kann
 *
 * minfo     - Mapping mit Eintraegen fuer:
 *                 P_HP (HP-Heilung),
 *                 P_SP (SP-Heilung),
 *                 P_FOOD (Saettigung),
 *                 P_DRINK (Fluessigkeit)
 *                 P_ALCOHOL (Alkoholisierung)
 *                 P_VALUE (Preis)
 *             Die Eintraege werden ueber eval_anything ausgewertet
 *             (siehe da)
 *
 * rate      - Heilrate (auch ueber eavl_anything) in Punkte / heart_beat()
 *
 * msg       - Meldung beim Essen.
 *             a) closure (wird mit funcall(msg,zahler,empfaenger)
 *                ausgewertet)
 *             b) string (wie closure: call_other(this_object...))
 *             c) array mit 2 strings: 1) fuer Essenden, 2) fuer andere
 *                siehe auch mess()
 *
 * refresh   - Mapping mit den moeglichen Eintraegen:
 *                 PR_USER : <Kontingent> ; <Update> (pro Spieler)
 *                 PR_ALL  : <Kontingent> ; <Update> (fuer alle)
 *             Es wird zunaechst geprueft, ob noch etwas fuer den
 *             (zahlenden!) Spieler selbst vorhanden ist wenn nein wird
 *             geschaut, ob das Kontingent fuer alle schon erschoepft ist.
 *             Sind beide Kontingente erschoepft, kann der Spieler das
 *             Bestellte nicht bekommen.
 *             Die Kontingente wird alle <Update> reset()s "aufgefrischt".
 *             <Kontingent> wird ueber eval_anything() ausgewertet.
 *             Alternativ kann man einen Int-Wert angeben. Dieser wird wie
 *             ([ PR_ALL : <wert> ; 1 ]) behandelt.
 *
 * delay     - Zahl der Sekunden, um die verzoegert die Heilung eintritt
 *             z.B. weil das Essen erst zubereitet werden muss.
 *             Ebenfalls ueber eval_anything()
 *
 * d_msg     - Meldung beim bestellen, falls Delay. Wie msg.
 */
varargs string AddToMenu(string menuetext, mixed ids, mapping minfo,
                         mixed rate, mixed msg, mixed refresh,
                         mixed delay, mixed d_msg)
{ string ident;
  int i;

  if ( !stringp(menuetext) || !ids || !mappingp(minfo) )
    return 0;

  if ( stringp(ids) )
    ids = ({ ids });
  else if ( !pointerp(ids) )
    return 0;

  ident = sprintf("menuentry%d",max_list);
  max_list++;

  /* Fuer schnelles Nachschlagen ein eigenes Mapping fuer Ids */
  for( i=sizeof(ids)-1;i>=0;i-- )
    id_list += ([ ids[i] : ident ]);

  if ( intp(refresh) && (refresh>0) )
    refresh = ([ PR_ALL : refresh; 1 ]);
  else if ( !mappingp(refresh) )
    refresh = 0;

  menu_list += ([ ident : menuetext; minfo; rate; msg; refresh;
                   delay; d_msg; ids ]);
  return ident;
}

// Diese Methode ist nur noch aus Kompatibilitaetsgruenden vorhanden und darf
// nicht mehr verwendet werden!!!
void AddFood(string nameOfFood, mixed ids, int price, int heal,
             mixed myFunction)
{
  if ( !nameOfFood || !ids || !price)
    return; /* Food not healing is ok ! */

  AddToMenu( nameOfFood,ids,
             ([ P_VALUE : price, P_FOOD : heal, P_HP : heal, P_SP : heal ]),
             ((heal>5)?5:heal), myFunction, 0,0,0);
}

// Diese Methode ist nur noch aus Kompatibilitaetsgruenden vorhanden und darf
// nicht mehr verwendet werden!!!
void AddDrink(string nameOfDrink, mixed ids, int price, int heal,
              int strength, int soak, mixed myFunction)
{
  if ( !nameOfDrink || !ids || !price )
    return;

  heal=heal/2;
  /* Kleine Korrektur, damit man in alten Pubs ueberhaupt trinken kann */
  AddToMenu(nameOfDrink,ids,
             ([ P_VALUE : price, P_DRINK : soak, P_ALCOHOL : strength,
                P_HP : heal, P_SP : heal ]),
             ((heal>5)?5:heal), myFunction,0,0,0);
}

int RemoveFromMenu(mixed ids) {
  int ret;

  if (stringp(ids))
    ids = ({ids});

  if (pointerp(ids)) {
    foreach (string id: ids) {
      // look if the id has a matching ident
      string ident = id_list[id];
      if (stringp(ident)) {
        // remove this ident-entry from the id-list ...
        m_delete(id_list, id);
        // ... and remove all others now too (so it won't bug later, if
        //     the wizard calling this method forgot an id)
        foreach (string listid: m_indices(id_list))
          if (id_list[listid] == ident)
            m_delete(id_list, listid);

        // now remove the ident from the menu_list
        if (member(menu_list, ident)) {
          ret++;
          m_delete(menu_list, ident);

          // decrease the ident-counter, if this entry was the last one
          int oldnum;
          if (sscanf(ident, "menuentry%d", oldnum) == 1 &&
              oldnum == (max_list-1))
            max_list--;
        }
      }
    }
  }
  // return removed entries
  return ret;
}

/* Zum Auswerten der Eintraege fuer Preis, Rate, HP...
 * a) integer-Wert    -> wird direkt uebernommen
 * b) mapping         -> Wird als RaceModifiere verstanden. Es wird der
 *                       Eintrag gewaehlt, der der Rasse des Spielers
 *                       entspricht, falls vorhanden, ansonsten der Eintrag
 *                       fuer 0.
 * c) Array           -> In erstem Element (muss Objekt oder dessen Filename
 *                       sein) wird die Funktion mit dem Namen im 2.Element
 *                       aufgerufen.
 * d) String          -> Die genannte Funktion wird in der Kneipe selbst
 *                       aufgerufen.
 * e) Closure         -> Die Closure wird ausgewertet.
 * Alle Funktionsaufrufe bekommen den essenden Spieler (bei Price und Delay
 * den bestellenden Spieler) als Argument uebergeben. Die Auswertung erfolgt
 * in der angegebenen Reihenfolge. Am Ende muss ein Intergerwert herauskommen
 */
int eval_anything(mixed what, object pl)
{ mixed re;

  if (intp(what))
    return what;

  if (mappingp(what) && pl)
  {
    re = what[pl->QueryProp(P_RACE)]||what[0];
  }

  if (re)
    what=re;

  if ( pointerp(what) && (sizeof(what)>=2)
      && ( objectp(what[0]) || stringp(what[0]) )
      && stringp(what[1]) )
    what = call_other(what[0],what[1],pl);

  if ( stringp(what) && function_exists(what,ME) )
    what = call_other(ME,what,pl);

  if ( closurep(what) )
    what = funcall(what,pl);

  if ( intp(what) )
    return what;

  return 0;
}

/* Diese Funktion ueberprueft, ob das Kontingent eines Menueeintrags
 * fuer einen Spieler erschoepft ist.
 */
string CheckAvailability(string ident, object zahler)
{ string uid;

  if ( !stringp(ident) || !member(menu_list,ident) || !objectp(zahler) )
    return 0;
  if ( !mappingp(menu_list[ident,PM_REFRESH]) )
    return PR_NONE;

  if ( !member(refresh_list,ident) )
    refresh_list += ([ ident : ([ ]) ]);

  if ( query_once_interactive(zahler) )
    uid=getuid(zahler);
  else
    uid=object_name(zahler);

  if ( member(menu_list[ident,PM_REFRESH],PR_USER) )
  {
    if ( !member(refresh_list[ident],uid) )
    {
      refresh_list[ident] += ([ uid : 0 ; refresh_count ]);
    }

    /* Kontingent des Zahlenden pruefen */
    if ( refresh_list[ident][uid,PRV_AMOUNT] <
          eval_anything(menu_list[ident,PM_REFRESH][PR_USER,PRV_AMOUNT],
                        zahler) )
      return uid;
  }

  if ( member(menu_list[ident,PM_REFRESH],PR_ALL) )
  {
    if ( !member(refresh_list[ident],PR_DEFAULT) )
    {
      refresh_list[ident] += ([ PR_DEFAULT : 0 ; refresh_count ]);
    }

    /* Kontingent der Allgemeinheit pruefen */
    if ( refresh_list[ident][PR_DEFAULT,PRV_AMOUNT] <
          eval_anything(menu_list[ident,PM_REFRESH][PR_ALL,PRV_AMOUNT],
                        zahler) )
      return PR_DEFAULT;
  }

  return 0;
}

/* Diese Funktion reduziert das Kontingent des Lebewesens uid beim
 * Menueeintrag ident um 1
 */
void DecreaseAvailability(string ident, string uid)
{
  if ( !stringp(ident) || !stringp(uid) )
    return;
  refresh_list[ident][uid,PRV_AMOUNT]++;
}

/* Diese Funktion sorgt dafuer, dass die Kontingente an limitierten
 * Sachen in regelmaessigen Abstaenden wiederhergestellt werden.
 */
static void UpdateAvailability()
{ int    i1,i2;
  string *ind1,*ind2,chk;

  /* Keine Refresh-Eintraege, kein Update */
  if ( !mappingp(refresh_list)
      || (i1=sizeof(ind1=m_indices(refresh_list)))<1 )
    return;

  /* Es muss jeder Menueeintrag, der in der refresh_list steht,
   * durchgegangen werden.
   */
  for ( --i1 ; i1>=0 ; i1-- )
  {
    if ( !mappingp(refresh_list[ind1[i1]])
        || (i2=sizeof(ind2=m_indices(refresh_list[ind1[i1]])))<1 )
      continue;

    /* Fuer jeden Menueeintrag muss jeder Spielereintrag durchgegangen
     * werden, der in dem entspr. mapping steht.
     */
    for ( --i2 ; i2>=0 ; i2-- ) // Alle Spieler
    {
      if ( ind2[i2]==PR_DEFAULT )
        chk = PR_ALL;
      else
        chk = PR_USER;

      if ( ( refresh_list[ind1[i1]][ind2[i2],PRV_REFRESH]
            + menu_list[ind1[i1],PM_REFRESH][chk,PRV_REFRESH] )
          <= refresh_count )
      {
        refresh_list[ind1[i1]][ind2[i2],PRV_AMOUNT]=0;
        refresh_list[ind1[i1]][ind2[i2],PRV_REFRESH]=refresh_count;
      }
    }
  }
}

mixed DBG(mixed o) {
  if(find_player("rumata"))
    tell_object(
        find_player("rumata"),
        sprintf("DBG: %O\n", o)
    );
  return 0;
}

/* Erweitert um die Moeglichkeit, Speise- oder Getraenke-Karte zu sehen. */
string menue_text(string str)
{ int i,sdr,sfo;
  string ident,res;
  string *fo=({}),*dr=({});

  if ( !max_list )
    return "Hier scheint es derzeit nichts zu geben.\n";

  if ( !stringp(str) || str=="" )
    str="alles";

  /* Fuers Menue entscheiden ob Drink oder Food */
  foreach(string id, string menuetext, mapping minfo: menu_list)
  {
    if (eval_anything(minfo[P_FOOD],this_player()))
      fo+=({ id });
    else
      dr+=({ id });
  }

  sdr = sizeof(dr);
  sfo = sizeof(fo);

  if ( member(({"alle","alles"}),str)!=-1)
  {
    if ( !sfo )
      str="drinks";
    else if ( !sdr )
      str="speise";
    else
    {
      /* Gemischte Karte */
      res = "Getraenke                    Preis alc | "+
            "Speisen                          Preis\n"+
            "---------------------------------------+-"+
            "--------------------------------------\n";

      for ( i=0 ; ( i<sdr || i<sfo ) ; i++ )
      {
        if ( i<sdr )
          res += sprintf("%-29.29s%5.5d  %c  | ",
                   menu_list[dr[i],PM_TEXT],
                   eval_anything(menu_list[dr[i],PM_INFO][P_VALUE],PL),
                   (eval_anything(menu_list[dr[i],PM_INFO][P_ALCOHOL],PL)>0) ? 'J' : 'N');
        else
          res += "                                       | ";

        if ( i<sfo )
          res += sprintf("%-33.33s%5.5d",
                   menu_list[fo[i],PM_TEXT],
                   eval_anything(menu_list[fo[i],PM_INFO][P_VALUE],PL));

        res += "\n";
      }

      return res;
    }
  }

  /* Reine Getraenkekarte */
  if ( member(({"getraenke","drinks","getraenk","trinken"}),str)!=-1 )
  {
    if ( !sdr )
      return "Hier gibt es leider nichts zu trinken.\n";

    res = "Getraenke                    Preis alc | "+
          "Getraenke                    Preis alc\n"+
          "---------------------------------------+-"+
          "--------------------------------------\n";

    for ( i=0 ; i<sdr ; i++ )
    {
      ident = dr[i];

      if ( !eval_anything(menu_list[ident,PM_INFO][P_FOOD], PL) )
        res += sprintf("%-29.29s%5.5d  %c%s",
                 menu_list[ident,PM_TEXT],
                 eval_anything(menu_list[ident,PM_INFO][P_VALUE],PL),
                 (eval_anything(menu_list[ident,PM_INFO][P_ALCOHOL],PL)>0) ? 'J' : 'N',
                 ((i%2)?"\n":"  | "));
    }

    if ( res[<1..<1]!="\n" )
      res += "\n";

    return res;
  }

  /* Reine Speisekarte */
  if ( member(({"speise","speisen","essen"}),str)!=-1 )
  {
    if ( !sfo )
      return "Hier gibt es leider nichts zu essen.\n";

    res = "Speisen                          Preis | "+
          "Speisen                          Preis\n"+
          "---------------------------------------+-"+
          "--------------------------------------\n";

    for ( i=0 ; i<sfo ; i++ )
    {
      ident = fo[i];
      if (eval_anything(menu_list[ident,PM_INFO][P_FOOD],PL) )
        res += sprintf("%-33.33s%5.5d%s",
                 menu_list[ident,PM_TEXT],
                 eval_anything(menu_list[ident,PM_INFO][P_VALUE],PL),
                 ((i%2)?"\n":" | "));
    }

    if ( res[<1..<1]!="\n" )
      res += "\n";

    return res;
  }

  return 0;
}

int menue(string str)
{ string txt;

  _notify_fail("Welchen Teil des Menues moechtest Du sehen?\n");
  if ( !stringp(txt=menue_text(str)) || sizeof(txt)<1 )
    return 0;
  write(txt);
  return 1;
}

/* Diese Funktion kann/soll bei Bedarf ueberladen werden, um simultane
 * Aenderungen des Mappings zu ermoeglichen (zu Beispiel wie es in guten
 * Tagen groesser Portionen gib, Hobbits per se mehr kriegen, ...
 */
mapping adjust_info(string ident, mapping minfo, object zahler,
                    object empfaenger)
{
  return 0;
}

/* Hier hats jede Menge neue Platzhalter */
string mess(string str,object pl)
{
  string dummy1, dummy2;

  if ( !pl )
    pl=PL;

  if ( !stringp(str) || str=="" )
    return str;

  str=implode(explode(str,"&&"),pl->name(WER,2));
  str=implode(explode(str,"&1&"),pl->name(WER,2));
  str=implode(explode(str,"&2&"),pl->name(WESSEN,2));
  str=implode(explode(str,"&3&"),pl->name(WEM,2));
  str=implode(explode(str,"&4&"),pl->name(WEN,2));
  str=implode(explode(str,"&1#"),capitalize(pl->name(WER,2)));
  str=implode(explode(str,"&2#"),capitalize(pl->name(WESSEN,2)));
  str=implode(explode(str,"&3#"),capitalize(pl->name(WEM,2)));
  str=implode(explode(str,"&4#"),capitalize(pl->name(WEN,2)));
  str=implode(explode(str,"&!"),pl->QueryPronoun(WER));
  str=implode(explode(str,"&5&"),pl->QueryPronoun(WER));
  str=implode(explode(str,"&6&"),pl->QueryPronoun(WESSEN));
  str=implode(explode(str,"&7&"),pl->QueryPronoun(WEM));
  str=implode(explode(str,"&8&"),pl->QueryPronoun(WEN));
  str=implode(explode(str,"&5#"),capitalize(pl->QueryPronoun(WER)));
  str=implode(explode(str,"&6#"),capitalize(pl->QueryPronoun(WESSEN)));
  str=implode(explode(str,"&7#"),capitalize(pl->QueryPronoun(WEM)));
  str=implode(explode(str,"&8#"),capitalize(pl->QueryPronoun(WEN)));

  return break_string(capitalize(str),78,"", BS_LEAVE_MY_LFS);
}

protected void _copy_menulist_values(mapping entryinfo, string ident) {
  /* Kopieren aller Werte ins minfo-Mapping, um Problemen bei Loeschung
     aus dem Weg zu gehen. Slow and dirty */
  entryinfo[PM_TEXT]      = deep_copy(menu_list[ident, PM_TEXT]);
  // PM_INFO is already flat in entryinfo
  entryinfo[PM_RATE_PUBMASTER]
                          = deep_copy(menu_list[ident, PM_RATE]);
  entryinfo[PM_SERVE_MSG] = deep_copy(menu_list[ident, PM_SERVE_MSG]);
  entryinfo[PM_REFRESH]   = deep_copy(menu_list[ident, PM_REFRESH]);
  // PM_DELAY is already evaluated in entryinfo
  entryinfo[PM_DELAY_MSG] = deep_copy(menu_list[ident, PM_DELAY_MSG]);
  entryinfo[PM_IDS]       = deep_copy(menu_list[ident, PM_IDS]);
}

int do_deliver(string ident, object zahler, object empfaenger,
               mapping entryinfo) {
  waiting -= ({ empfaenger,0 });

  /* Empfaenger muss natuerlich noch da sein */
  if ( !objectp(empfaenger) || !present(empfaenger) )
    return 0;

  /* Zahler wird nur wegen der Abwaertskompatibilitaet gebraucht */
  if ( !objectp(zahler) )
    zahler = empfaenger;

  // alte Pubs, die do_deliver irgendwie selbst aufrufen, sollten
  // mit der Zeit korrigiert werden
  if(!mappingp(entryinfo))
    raise_error("Pub ruft do_deliver() ohne sinnvolle Argumente auf.\n");
  if(!member(entryinfo, PM_RATE_PUBMASTER)) {
    if(!member(menu_list, ident))
      raise_error("Pub ruft do_deliver() mit geloeschtem Getraenk und "
                  "teilweisen Argumenten auf!\n");

    _copy_menulist_values(entryinfo, ident);
    call_out(#'raise_error, 1,
             "Pub ruft do_deliver() nur mit teilweisen Argumenten auf.\n");
  }

  entryinfo[PM_RATE_PUBMASTER] = eval_anything(entryinfo[PM_RATE_PUBMASTER],
                                               empfaenger);
  entryinfo[P_HP]              = eval_anything(entryinfo[P_HP], empfaenger);
  entryinfo[P_SP]              = eval_anything(entryinfo[P_SP], empfaenger);

  /* Ueberpruefen, ob Heilmoeglichkeit legal */
  if ( query_once_interactive(empfaenger)
      && ((PUBMASTER->RegisterItem(entryinfo[PM_TEXT], entryinfo))<1) ) {
     tell_object(empfaenger,
       "Mit diesem Getraenk/Gericht scheint etwas nicht in Ordnung "+
       "zu sein.\nVerstaendige bitte den Magier, der fuer diesen "+
       "Raum verantwortlich ist.\n");
     return -4;
  }

  if ( QueryProp(P_NPC_FASTHEAL) && !query_once_interactive(empfaenger) ) {
    entryinfo[H_DISTRIBUTION] = HD_INSTANT;
  }
  else {
    entryinfo[H_DISTRIBUTION] = entryinfo[PM_RATE_PUBMASTER];
  }
  empfaenger->consume(entryinfo);

  /* Meldung ausgeben */
  /* Hinweis: Da die ausfuehrenden Funktionen auch ident und minfo
   * uebergeben bekommen, kann man hier auch ueber adjust_info oder
   * an anderer Stelle zusaetzliche Informationen uebergeben...
   */
  if (closurep(entryinfo[PM_SERVE_MSG]) )
    funcall(entryinfo[PM_SERVE_MSG], zahler, empfaenger, ident, entryinfo);
  else if (stringp(entryinfo[PM_SERVE_MSG]) &&
           function_exists(entryinfo[PM_SERVE_MSG],ME))
    call_other(ME, entryinfo[PM_SERVE_MSG], zahler, empfaenger, ident,
                   entryinfo);
  else if (pointerp(entryinfo[PM_SERVE_MSG]) &&
           sizeof(entryinfo[PM_SERVE_MSG])>=2)  {
    if (stringp(entryinfo[PM_SERVE_MSG][0]) &&
        sizeof(entryinfo[PM_SERVE_MSG][0]))
      tell_object(empfaenger,
        mess(entryinfo[PM_SERVE_MSG][0]+"\n", empfaenger));
    if (stringp(entryinfo[PM_SERVE_MSG][1]) &&
        sizeof(entryinfo[PM_SERVE_MSG][1]))
      tell_room(environment(empfaenger)||ME,
        mess(entryinfo[PM_SERVE_MSG][1]+"\n",empfaenger),
        ({empfaenger}) );
  }

  return 1;
}

/* Testet, ob genug Geld zur Verfuegung steht.
 * Falls die Bonitaet anderen Beschraenkungen unterliegt, als
 * dass der Zahler genug Geld dabei hat, muss diese Methode
 * ueberschrieben werden.
 * Rueckgabewerte:
 * -2 : Out of Money
 *  0 : Alles OK.
 * Rueckgabewerte != 0 fuehren zu einem Abbruch der Bestellung
 */
int CheckSolvency(string ident, object zahler, object empfaenger,
                   mapping entryinfo)
{
  if ( (zahler->QueryMoney())<entryinfo[P_VALUE] )
  {
    string res;
    if ( !stringp(res=QueryProp(P_PUB_NO_MONEY)) )
      res = "Das kostet %d Goldstuecke, und Du hast nicht so viel!\n";
    tell_object(zahler,sprintf(res, entryinfo[P_VALUE]));
    return -2;
  }
  return 0;
}

/* Fuehrt die Bezahlung durch.
 * Falls die Bezahlung anders erfolgt, als durch Abzug des Geldes vom Zahler,
 * muss diese Methode ueberschrieben werden
 * Rueckgabewerte:
 * Anzahl der Muenzen, die im Pub landen und bei Reset in die Zentralbank
 * eingezahlt werden
 */
int DoPay(string ident, object zahler, object empfaenger, mapping entryinfo)
{
  zahler->AddMoney(-entryinfo[P_VALUE]);
  return entryinfo[P_VALUE];
}

/* Rueckgabewerte:
 * -6 : Nicht vorraetig
 * -5 : Wirt nicht anwesend
 * -4 : Illegales Getraenk/Gericht. Ausgabe verweigert.
 *      Nur bei sofortiger Lieferung...
 * -3 : Empfaenger bereits voll
 * -2 : Out of Money
 * -1 : spendieren ignoriert
 *  0 : Empfaenger ausgeflogen (sollte eigentlich nicht passieren)
 *  1 : Alles OK.
 */
int consume_something(string ident, object zahler, object empfaenger) {
  if ( !objectp(zahler) )
    zahler=PL;

  if ( !objectp(empfaenger) )
    empfaenger=PL;

  /* Die Abfrage auf anwesenden Wirt erfolgt NUR an dieser Stelle, damit */
  /* kein Spieler darunter leiden muss, wenn jemand anderes zwischen     */
  /* Bestellung und Lieferung den Wirt meuchelt.                         */
  if ( stringp(QueryProp(P_KEEPER)) && !present(QueryProp(P_KEEPER), ME))
  {
    string res = QueryProp(P_PUB_NO_KEEPER);
    if ( !stringp(res) ) {
      res = "Es ist niemand anwesend, der Dich bedienen koennte.\n";
    }
    tell_object(zahler,res);
    return -5;
  }

  /* Spendiert und ignoriert? */
  if ( zahler!=empfaenger )
  {
    mixed res = ({"spendiere"});
    if ( eval_anything(menu_list[ident,PM_INFO][P_DRINK],empfaenger) )
      res += ({"spendiere.getraenke"});
    if ( eval_anything(menu_list[ident,PM_INFO][P_FOOD],empfaenger) )
      res += ({"spendiere.essen"});
    if ( eval_anything(menu_list[ident,PM_INFO][P_ALCOHOL],empfaenger) )
      res += ({"spendiere.alkohol"});
    if ( empfaenger->TestIgnoreSimple(res) )
    {
      tell_object(zahler,
        empfaenger->Name(WER)+" will das nicht.\n");
      return -1;
    }
  }

  /* Hier kann das Info-Mapping erst mal als ganzes angepasst werden. */
  mapping xinfo;
  mapping entryinfo = deep_copy(menu_list[ident, PM_INFO]);
  if ( (xinfo=adjust_info(ident,entryinfo,zahler,empfaenger)) &&
       mappingp(xinfo) )
    entryinfo += xinfo;

  /* Genug Geld vorhanden? */
  entryinfo[P_VALUE] = eval_anything(entryinfo[P_VALUE], zahler);
  {
    int res = CheckSolvency(ident, zahler, empfaenger, entryinfo);
    if (res != 0) return res;
  }

  string avb;
  if ( !stringp(avb=CheckAvailability(ident, zahler)) )
  {
    string res = QueryProp(P_PUB_UNAVAILABLE);
    if ( !stringp(res) )
      res = "Davon ist leider nichts mehr da.\n";
    tell_object(zahler,res);
    return -6;
  }

  /* Textausgabe beim spendieren */
  if ( empfaenger!=zahler)
  {
    tell_room(environment(empfaenger)||ME,
      zahler->Name(WER)+" spendiert "+empfaenger->name(WEM)+" etwas.\n",
      ({zahler, empfaenger}) );
    tell_object(empfaenger,
      zahler->Name(WER)+" spendiert Dir etwas.\n");
    tell_object(zahler,
      "Du spendierst "+empfaenger->name(WEM)+" etwas.\n");
  }

  /* Testen, ob mans noch essen / trinken kann */
  /* Die int-Werte werden in minfo uebernommen fuer die Auswertung */
  /* im Pubmaster. */
  entryinfo[P_FOOD]    = eval_anything(entryinfo[P_FOOD],   empfaenger);
  entryinfo[P_ALCOHOL] = eval_anything(entryinfo[P_ALCOHOL],empfaenger);
  entryinfo[P_DRINK]   = eval_anything(entryinfo[P_DRINK],  empfaenger);

  int result = empfaenger->consume(entryinfo, 1);
  if (result < 0) {
    if (result & HC_MAX_FOOD_REACHED)
      tell_object(empfaenger,
                  "Du bist zu satt, das schaffst Du nicht mehr.\n");
    else if (result & HC_MAX_DRINK_REACHED)
      tell_object(empfaenger,
                  "So viel kannst Du im Moment nicht trinken.\n");
    else if (result & HC_MAX_ALCOHOL_REACHED)
      tell_object(empfaenger,
                  "Soviel Alkohol vertraegst Du nicht mehr.\n");
    return -3;
  }

  /* Gezahlt wird auch sofort */
  sum += DoPay(ident, zahler, empfaenger, entryinfo);

  /* FPs gibts auch sofort */
  if (zahler == empfaenger)
    GiveEP(EP_PUB,menu_list[ident,PM_IDS][0]);

  /* Falls die Anzahl des Bestellten beschraenkt ist, muss diese natuerlich
   * angepasst werden.
   */
  if ( avb!=PR_NONE )
    DecreaseAvailability(ident,avb);

  /* Gibt es eine Zeitverzoegerung zwischen Bestellen und Servieren? */
  entryinfo[PM_DELAY_PUBMASTER] = eval_anything(menu_list[ident, PM_DELAY], zahler);

  // alle fuer einen Drink notwendigen Werte kopieren
  _copy_menulist_values(entryinfo, ident);

  if (entryinfo[PM_DELAY_PUBMASTER]<=0)
    return do_deliver(ident,zahler,empfaenger,entryinfo);

  /* Bestell-Meldung ausgeben */
  if (closurep(entryinfo[PM_DELAY_MSG]))
    funcall(entryinfo[PM_DELAY_MSG], zahler, empfaenger,ident, entryinfo);
  else if (stringp(entryinfo[PM_DELAY_MSG]) &&
           function_exists(entryinfo[PM_DELAY_MSG],ME))
    call_other(ME, entryinfo[PM_DELAY_MSG], zahler, empfaenger, ident,
               entryinfo);
  else if (pointerp(entryinfo[PM_DELAY_MSG]) &&
           sizeof(entryinfo[PM_DELAY_MSG])>=2) {
    if (stringp(entryinfo[PM_DELAY_MSG][0]) &&
       sizeof(entryinfo[PM_DELAY_MSG][0]))
      tell_object(empfaenger,
        mess(entryinfo[PM_DELAY_MSG][0]+"\n",empfaenger));
    if (stringp(entryinfo[PM_DELAY_MSG][1]) &&
        sizeof(entryinfo[PM_DELAY_MSG][1]))
      tell_room(environment(empfaenger)||ME,
        mess(entryinfo[PM_DELAY_MSG][1]+"\n",empfaenger),
        ({empfaenger}) );
  }

  waiting += ({ empfaenger });
  call_out("do_deliver", entryinfo[PM_DELAY_PUBMASTER],
           ident, zahler, empfaenger, entryinfo);

  return 1;
}

/* Rueckgabewere: 0: Nicht im Menue gefunde, 1 sonst */
int search_what(string str,object zahler,object empfaenger)
{ string ident;

  if ( member(waiting,empfaenger)!=-1 )
  {
    if ( PL==empfaenger )
      write("Du wartest doch noch auf etwas!\n");
    else
      write(empfaenger->Name(WER,2)+" wartet noch auf etwas.\n");
    return 1;
  }

  str = lower_case(str);
  if ( ident=id_list[str] )
  {
    consume_something(ident,zahler,empfaenger);
    return 1;
  }

  return 0;
}

// Neue Version von Mesi:
int spendiere(string str)
{
   _notify_fail("spendiere <spieler> <drink>\n");

   if ( !stringp(str) || str=="" )
     return 0;

   string who,what;
   int whoidx;

   if (sscanf(str,"%s %d %s",who,whoidx,what)!=3
    && sscanf(str,"%s %s",who,what)!=2)
      return 0;
  object target=present(who, whoidx);
  if(!target && this_player()) target=present(who, whoidx, this_player());
   if ( !target || !living(target) )
   {
     write("Das Lebewesen ist nicht hier...\n");
     return 1;
   }

   notify_fail((string)QueryProp(P_PUB_NOT_ON_MENU)||"So etwas gibt es hier nicht!\n");

   return search_what(what,PL,target);
}

int bestelle(string str)
{
  notify_fail((string)QueryProp(P_PUB_NOT_ON_MENU));

  if ( !stringp(str) )
    return 0;

  return search_what(str,PL,PL);
}

int pubinit()
{ string  *liste,ident,fn;
  int     si,erg,max;
  mapping minfo,xinfo;

  if ( !PL || !IS_WIZARD(PL) )
    return 0;

  si=sizeof(liste=sort_array(m_indices(menu_list),#'<));
  if ( si<1 )
    return notify_fail("Keine Gerichte/Getraenke vorhanden.\n"),0;

  fn=old_explode(object_name(ME),"#")[0];
  printf("\n%'_'|30s %3s %3s %3s %5s %2s %2s %3s %3s %4s %3s\n",
    "ITEM","ALC","DRI","FOO","VALUE","RT","DL","_HP","_SP","TEST","MAX");
  for ( --si ; si>=0 ; si-- )
  {
    ident=liste[si];
    minfo=deep_copy(menu_list[ident,PM_INFO]);

    if ( (xinfo=adjust_info(ident,minfo,PL,PL)) && mappingp(xinfo) )
      minfo+=xinfo;

    minfo[P_VALUE]   = eval_anything(minfo[P_VALUE],            PL);
    minfo[P_FOOD]    = eval_anything(minfo[P_FOOD],             PL);
    minfo[P_ALCOHOL] = eval_anything(minfo[P_ALCOHOL],          PL);
    minfo[P_DRINK]   = eval_anything(minfo[P_DRINK],            PL);
    minfo[PM_DELAY_PUBMASTER]
                     = eval_anything(menu_list[ident,PM_DELAY], PL);
    minfo[PM_RATE_PUBMASTER]
                     = eval_anything(menu_list[ident,PM_RATE],  PL);
    minfo[P_HP]      = eval_anything(minfo[P_HP],               PL);
    minfo[P_SP]      = eval_anything(minfo[P_SP],               PL);
    erg=PUBMASTER->RegisterItem(menu_list[ident,0],minfo);
    max=PUBMASTER->CalcMax(minfo,fn);

    printf("%-'..'30.30s %3d %3d %3d %5d %2d %2d %3d %3d %|4s %3d\n",
      menu_list[ident,PM_TEXT],
      minfo[P_ALCOHOL], minfo[P_DRINK], minfo[P_FOOD],
      minfo[P_VALUE],
      minfo[PM_RATE_PUBMASTER],
      minfo[PM_DELAY_PUBMASTER],
      minfo[P_HP], minfo[P_SP],
      ( erg ? "OK" : "FAIL" ),max);
  }
  write("Done.\n");
  return 1;
}

void reset()
{
  if ( sum>0 )
    ZENTRALBANK->PayIn(sum);
  sum=0;
  refresh_count++;
  UpdateAvailability();
}

void add_gluehwein()
{
  if ( ctime(time())[4..6]=="Dec" )
    AddToMenu( "Gluehwein",({"gluehwein"}),([
      P_VALUE   : 80,
      P_DRINK   :  5,
      P_ALCOHOL : 20,
      P_HP      : 15,
      P_SP      : 15 ]),2,({
      ("Du trinkst ein Glas Gluehwein, an dem Du Dir beinahe die Zunge "+
       "verbrennst.\n"),
      ("&& bestellt ein Glas Gluehwein und verbrennt sich beim\n"+
       "Trinken beinahe die Zunge.\n") }), 0, 0, 0);
}

void add_std_drinks()
{
  if ( QueryProp(P_NO_STD_DRINK) )
    return ;
  add_gluehwein();
}

mapping query_menulist()
{
  return deep_copy(menu_list);
}

string *query_drinks()
{
  string *dr=({});
  foreach( string ident, string menuetext, mapping minfo: menu_list) {
    if (eval_anything(minfo[P_DRINK], 0))
      dr += ({ ident });
  }
  return dr;
}

string *query_food()
{
  string *fo=({});
  foreach( string ident, string menuetext, mapping minfo: menu_list) {
    if (eval_anything(minfo[P_FOOD], 0))
      fo += ({ ident });
  }
  return fo;
}
