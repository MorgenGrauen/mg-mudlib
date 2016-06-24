// MorgenGrauen MUDlib
//
// player/description.c -- player description handling
//
// $Id: description.c 8755 2014-04-26 13:13:40Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/living/description";

#include <thing/language.h>
#include <player/description.h>
#include <living/clothing.h>
#include <properties.h>
#include <wizlevels.h>
#include <defines.h>
#include <combat.h>
#include <sys_debug.h>

#define NEED_PROTOTYPES
#include <player/command.h>

protected void create()
{
  ::create();
  Set(P_NAME, NOSETMETHOD, F_SET_METHOD);
  Set(P_PRESAY, SAVE, F_MODE_AS);
  Set(P_TITLE, SAVE, F_MODE_AS);
  Set(P_EXTRA_LOOK, SAVE, F_MODE_AS);
  Set(P_GENDER, SAVE, F_MODE_AS);
  Set(P_SIZE, SAVE, F_MODE_AS);
  Set(P_INFO, NOSETMETHOD, F_SET_METHOD);
  // Avatar-URIs speichern. Ausserdem hat an denen keiner rumzufummeln.
  Set(P_AVATAR_URI, SAVE|SECURED, F_MODE_AS);
}

string _query_info()
{
  string info;
  info = Query(P_INFO);
  if(!info)info=="";
  info = (string)query_wiz_level(this_object());
  if(IS_GOD(this_object()))
    return info+" [Gott]\n";
  if(IS_ARCH(this_object()))
    return info+" [Erzmagier]\n";
  if(IS_ELDER(this_object()))
    return info+" [Weiser]\n";
  if(IS_LORD(this_object()))
    return info+" [Regionschef]\n";
  if(IS_SPECIAL(this_object()))
    return info+" [Special]\n";
  if(IS_DOMAINMEMBER(this_object()))
    return info+" [Regionsmitarbeiter]\n";
  if(IS_WIZARD(this_object()))
    return info+" [Magier]\n";
  if(IS_LEARNER(this_object()))
    return info+" [Magieranwaerter]\n";
  info = QueryProp(P_LEVEL);
  if(IS_SEER(this_object()))
    return info+" [Seher]\n";
  return info+" [Spieler]\n";
}

int _query_size() {
  if (QueryProp(P_FROG))
    return 20;
  return Query(P_SIZE);
}

// short() -- get the short description of a player
string short()
{
  string answer;
  string title;

  if(QueryProp(P_INVIS))
    if(interactive(previous_object()) && IS_LEARNING(previous_object()))
      return "("+QueryProp(P_NAME)+") \n";
    else
      return 0;

  if(QueryProp(P_GHOST))
  {
    if (QueryProp(P_FROG))
      return "Der Geist eines kleinen Frosches namens "+QueryProp(P_NAME)+
       ".\n";
    else
      return "Der Geist von "+QueryProp(P_NAME)+".\n";
  }

  if (QueryProp(P_FROG))
    return "Ein kleiner Frosch namens "+QueryProp(P_NAME)+".\n";

  answer = QueryProp(P_PRESAY) + QueryProp(P_NAME);
  if (QueryProp(P_ARTICLE)) answer=QueryArticle(0,0)+answer;
  if((title=QueryProp(P_TITLE)) && title != "") answer += " " + title;
  if(!interactive(ME)) answer += " (netztot)";
  return answer+".\n";
}

private string andlist(object *arr) {
  string *tmp;
  if(!pointerp(arr)) return "";
  if(sizeof(tmp = map_objects(arr, "name", WEN)))
    return(CountUp(tmp));
  return "";
}

// gibt fuer nicht-Invis-Objekte den Namen zurueck
private string get_vis_name(object ob) {
  if (ob->QueryProp(P_INVIS))
    return 0;
  return (ob->name(WEN));
}

varargs string long()
{
  string exl, descr, tmp, size_descr;
  object ob;
  mixed trans, w, a, r;
  int per;
  string fill, fill2;
  /* fuer groessenvergleich */
  string comparesize, pers1, pers2;
  int relation;

  per=1000*QueryProp(P_SIZE)/QueryProp(P_AVERAGE_SIZE);
  switch(per)
  {
    case 0..800: size_descr="ziemlich winzig"; break;
    case 801..850: size_descr="eher winzig"; break;
    case 851..900: size_descr="recht klein"; break;
    case 901..950: size_descr="eher klein"; break;
    case 951..1050: size_descr="mittelgross"; break;
    case 1051..1100: size_descr="relativ gross"; break;
    case 1101..1150: size_descr="ziemlich gross"; break;
    case 1151..1200: size_descr="sehr gross"; break;
    default: size_descr="riesig"; break;
  }
#define RassenString() ((QueryProp(P_FROG) ? "en Frosch" :\
       (!QueryProp(P_GENDER)?" ":QueryProp(P_GENDER)==2?"e ":"en ")+\
       (pointerp(QueryProp(P_RACESTRING))?\
        QueryProp(P_RACESTRING)[WEN]:QueryProp(P_RACE))))
  fill2=fill=0;
  if (QueryProp(P_AVERAGE_SIZE)<170)
  {
    if (per<950)
      fill="selbst fuer ein"+RassenString()+" ";
    else
      if (per>1050)
  fill2=", wenn auch nur fuer ein"+RassenString();
  }
  else
  {
    if (QueryProp(P_AVERAGE_SIZE)>170)
      if (per>1050)
  fill="sogar fuer ein"+RassenString()+" ";
      else
  if (per<950)
    fill2=", wenn auch nur fuer ein"+RassenString();
  }
  if (!fill&&!fill2) fill="fuer ein"+RassenString()+" ";
  descr = "Das ist "+name(WER,1)+". "+capitalize(QueryPronoun())+" ist "+
    (fill||"")+size_descr+(fill2||"")+".\n";

  if(this_player()) {
    /* groessenvergleich_anfang (NEU) */
    pers1 = QueryPronoun(WER);
    pers2 = QueryPronoun(WEM);
 
    // || falls TP keine Groesse gesetzt hat... Warum auch immer...
    relation = (QueryProp(P_SIZE)*100) / 
                  (this_player()->QueryProp(P_SIZE) || 1);
    switch (relation) 
     {
     case   0 ..  25 : comparesize = "Damit gibt "+pers1+" einen guten"
                                     " Fusschemel fuer Dich ab"; 
                                     break;
     case  26 ..  50 : comparesize = "Damit reicht "+pers1+" Dir nicht mal bis"
                                     " zur Huefte"; 
                                     break;
     case  51 ..  75 : comparesize = "Damit kannst Du "+pers2+" locker auf den"
                                     " Kopf spucken"; 
                                     break;
     case  76 ..  90 : comparesize = "Damit ist "+pers1+" einen Kopf kleiner"
                                     " als Du"; 
                                     break;
     case  91 .. 110 : comparesize = "Damit hat "+pers1+" etwa Deine Groesse";
                                     break;  
     case 111 .. 120 : comparesize = "Damit ist "+pers1+" einen Kopf groesser" 
                                     " als Du";
                                     break;
     case 121 .. 150 : comparesize = "Damit holst Du Dir einen steifen Nacken,"
                                     " wenn Du "+pers2+" in die Augen siehst";
                                     break;
     case 151 .. 200 : comparesize = "Damit versperrt "+pers1+" Dir absolut"
		                     " die Sicht"; 
                                     break;
     case 201 .. 300 : comparesize = "Damit waere jeder Schlag von Dir ein" 
                                     " Tiefschlag";
                                     break;
     default         : comparesize = "Damit kannst Du "+pers2+" bequem zwischen"
                                     " den Beinen durchlaufen";
                                     break;
     }
  
     descr+=comparesize+".\n";
     /* groessenvergleich_ende (NEU) */
   }

  if(QueryProp(P_GHOST)) return descr;

  trans = QueryProp(P_TRANSPARENT); SetProp(P_TRANSPARENT, 0);
  descr += ::long(); SetProp(P_TRANSPARENT, trans);

  // Waffen, Ruestungen/Kleidung und Sonstiges ermitteln
  w = ({QueryProp(P_WEAPON), QueryProp(P_PARRY_WEAPON)}) - ({0});
  a = QueryProp(P_ARMOURS) + QueryProp(P_CLOTHING) - ({0});
  r = all_inventory(ME) - w - a; //Rest logischerweise

  // rest noch nach sichbarkeit von ausserhalb des SPielers filtern.
  r=map(r,function string (object ob) {
	  if(sizeof(all_inventory(ob)) || ob->QueryProp(P_LIGHT) || 
	     ob->QueryProp(P_LIGHTED) || ob->QueryProp(P_SHOW_INV))
	    return(get_vis_name(ob));
          return(0); } ) - ({0});

  // Invis-Objekte ausfiltern und Namen ermitteln, anschliessend sind in w, a
  // und r je ein Array von Strings
  w = map(w, #'get_vis_name) - ({0});
  a = map(a, #'get_vis_name) - ({0});

  return descr +
         (QueryProp(P_TRANSPARENT) ?
          break_string(capitalize(name(WER, 1))+" hat "
                      +(sizeof(a)?CountUp(a):"keine Ruestung")+" an"
                      +(sizeof(r)?", ":" und ")
                      +(sizeof(w)?CountUp(w):"keine Waffe")+" gezueckt"
          +(sizeof(r)?" und traegt "+CountUp(r):"")+".", 78): "");
}


// **** local property methods
static mixed _query_presay()
{
  string presay;
  if((presay = Query(P_PRESAY)) && (presay != "")) return presay + " ";
  return "";
}

static string _query_name()
{
  return capitalize(Query(P_NAME) || "NoName");
}
// ****

// Local commands
static mixed _query_localcmds() {
  return
    ({
      ({"avatar","set_avataruri",0,0}),
    });
}

int set_avataruri(string arg) {
  arg = _unparsed_args(0);
  if (!arg || !sizeof(arg)) {
    string uri = QueryProp(P_AVATAR_URI);
    if (stringp(uri))
      tell_object(ME,
          "Aktuelle Avatar-URI: " + uri + "\n");
    else
      tell_object(ME, "Du hast keine Avatar-URI eingestellt.\n");
  }
  else if (arg == "keine") {
      SetProp(P_AVATAR_URI, 0);
      tell_object(ME, "Deine Avatar-URI wurde geloescht.\n");
  }
  else if (sizeof(arg) > 512)
      tell_object(ME, "Deine neue Avatar-URI ist zu lang!\n");
  else {
      SetProp(P_AVATAR_URI, arg);
      tell_object(ME,
          "Aktuelle Avatar-URI: " + arg + "\n");
  }
  return 1;
}

