// MorgenGrauen MUDlib
//
// lepmaster.c -- Master, welcher aus scoremaster, explorationmaster,
//                questmaster und dem Spielerobjekt Informationen
//                ausliest und aus diesen die Levelpunkte des Spieler
//                berechnet
//                Weiters derjenige, der die Seheranforderungen 
//                prueft. 
//
// $Id: lepmaster.c 9168 2015-03-05 00:04:22Z Zesstra $

// Hier kommen Funktionen fuer die Levelpunkte
#pragma strict_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma verbose_errors
#pragma combine_strings
//#pragma pedantic
//#pragma range_check
#pragma warn_deprecated

#include <config.h>
#include <properties.h>
#include <new_skills.h>
#include <exploration.h>
#include <questmaster.h>
#include <scoremaster.h>
#include <wizlevels.h>

#include "lepmaster.h"

// Das Programm zur Foerderung der kleinen Level aktivieren. ;-)
#define __PFOERKL__ 1

// die ersten 8 Level, d.h. Level 1 bis 8 entsprechen diesen LEPs.
int *LOW_LEVEL_LEP_LIST = ({100,120,140,180,220,300,360,420}); // 9. Level: 500 LEP

#define REQ_TEXT1  ([0:"unglaublich viel", \
                  1:"unglaublich viel", \
		  2:"enorm viel", \
		  3:"sehr viel", \
		  4:"viel", \
		  5:"einiges", \
		  6:"etwas", \
		  7:"wenig", \
                  8:"sehr wenig", \
                  9:"kaum etwas" ])

#define REQ_TEXT2  ([0:"unglaublich viele", \
                  1:"unglaublich viele", \
		  2:"enorm viele", \
		  3:"sehr viele", \
		  4:"viele", \
		  5:"einige", \
		  6:"ein paar", \
		  7:"wenige", \
                  8:"sehr wenige", \
                  9:"kaum" ])

#define DEBUG(x,y)
//#define DEBUG(x,y) if (find_player("zook")) tell_object(find_player("zook"),sprintf(x,y))


void create()
{
  seteuid(getuid());
}

int QueryLEPForPlayer(object pl)
{
  int ret, val, i, l;

  if (extern_call() && !IS_ARCH(geteuid(previous_object())))
    return -1;
  if (!pl || !query_once_interactive(pl))
    return -2;

  //  Grundoffset 100, da man mit Level 1 statt 0 startet
  ret = 100;

  // Beitrag A: Stupse von NPC-Erstkills
  ret += (int)SCOREMASTER->QueryKillPoints(pl);

  DEBUG("Nach KP: ret = %d\n", ret);

  // Beitrag B: Stupse von geloesten Miniquests
  ret += (int)QM->QueryMiniQuestPoints(pl);

  DEBUG("Nach MQP: ret = %d\n", ret);

  // Beitrag C: Questpunkte
  //  werden 1:1 uebernommen;
  ret += (int)pl->QueryProp(P_QP);

  DEBUG("Nach QP: ret = %d\n", ret);

  // Beitrag D: Erfahrungspunkte
  //  Stupse = XPs ^ 0.32
  val = (int)pl->QueryProp(P_XP);

  if (val<1) l=0;
  else l=to_int(exp(log(to_float(val))*0.32));

  ret += l;

  DEBUG("Nach XP: ret = %d\n", ret);

  // Beitrag E: Zaubertraenke
  //  Gefundene Traenke geben 5 LEP
  //  Die Heiltraenke geben zusaetzlich 10+20+30+40 LEP
  i = 80 - (val = sizeof((int *)pl->QueryProp(P_POTIONROOMS)));

  ret += 5*i + ([ 0: 100, 1: 60, 2: 30, 3: 10])[val];

  // Beitrag F: Forscherpunkte
  //  Pro FP gibt es 6 Stufenpunkte
  ret += 6 * (int)EPMASTER->QueryExplorationPoints(pl);

  DEBUG("Nach FP: ret = %d\n", ret);

  // Beitrag G: Gildeneinstufung
  //  Maximale Gildeneinstufung (10000) entspricht vier Leveln
  ret += ((int)pl->QueryProp(P_GUILD_RATING))/25;

  DEBUG("Nach GR: ret = %d\n", ret);

  // Ausgabe gibt es nur in 20er-Schritten
  ret -= ret%20;

  return (ret > 100) ? ret : 100;

}

nomask int QueryLEP()
{
  if (!previous_object())
    return 0;
  return QueryLEPForPlayer(previous_object());
}

nomask int QueryLevel(int lep) {
  if (lep<=0)
    lep=QueryLEP();

  if (lep<=0)
    return 0;

#ifdef __PFOERKL__
  // normale Level alle 100 LEP
  if (lep >= 500)
    return lep/100 + 4;
  else {
    // level mit weniger LEP am Anfang (1-8)
    // Level aus der LOW_LEVEL_LEP_LIST raussuchen.
    int lev = sizeof(LOW_LEVEL_LEP_LIST) - 1;
    for ( ; lev >= 0; --lev) {
      if (LOW_LEVEL_LEP_LIST[lev] <= lep)
        break;  // gefunden
    }
    return lev+1;
  }
#else
  return lep/100;
#endif // __PFOERKL__
  return 0;
}

// Wieviele LEP fehlen zum naechsten Level?
nomask int QueryNextLevelLEP(int lvl, int lep) {
  int needed;

  if (QueryLevel(lep) > lvl)
    return 0;

#ifdef __PFOERKL__
  if (lvl < sizeof(LOW_LEVEL_LEP_LIST))
    needed = LOW_LEVEL_LEP_LIST[lvl];
  else
    needed = (lvl-3) * 100; // (lvl + 1 - 4) * 100
#else
  needed = (lvl+1) * 100;
#endif // __PFOERKL__

  // needed sind jetzt die insgesamt benoetigten LEP. Vorhandene abziehen.
  needed -= lep;
  // nix < 0 zurueckliefern
  return max(needed,0);
}

string QueryForschung()
{
  int max, my, avg;
  string ret;

  if ((my=(int)EPMASTER->QueryExplorationPoints(getuid(previous_object()))) < MIN_EP)
    return "Du kennst Dich im "MUDNAME" so gut wie gar nicht aus.\n";

  my *= 100;
  max = my/(int)EPMASTER->QueryMaxEP();
  avg = my/(int)EPMASTER->QueryAverage();

  ret = "Verglichen mit Deinen Mitspielern, kennst Du Dich im "MUDNAME" ";
  switch(avg) {
  case 0..10:
    ret += "kaum";
    break;
  case 11..40:
    ret += "aeusserst schlecht";
    break;
  case 41..56:
    ret += "sehr schlecht";
    break;
  case 57..72:
    ret += "schlecht";
    break;
  case 73..93:
    ret += "unterdurchschnittlich";
    break;
  case 94..109:
    ret += "durchschnittlich gut";
    break;
  case 110..125:
    ret += "besser als der Durchschnitt";
    break;
  case 126..145:
    ret += "recht gut";
    break;
  case 146..170:
    ret += "ziemlich gut";
    break;
  case 171..210:
    ret += "gut";
    break;
  case 211..300:
    ret += "sehr gut";
    break;
  case 301..400:
    ret += "ausserordentlich gut";
    break;
  case 401..500:
    ret += "unheimlich gut";
    break;
  default:
    ret += "einfach hervorragend";
    break;
  }
  ret += " aus.\nAbsolut gesehen ";

  switch(max) {
  case 0..5:
    ret += "kennst Du nur wenig vom "MUDNAME".";
    break;
  case 6..10:
    ret += "solltest Du Dich vielleicht noch genauer umsehen.";
    break;
  case 11..17:
    ret += "bist Du durchaus schon herumgekommen.";
    break;
  case 18..25:
    ret += "hast Du schon einiges gesehen.";
    break;
  case 26..35:
    ret += "bist Du schon weit herumgekommen.";
    break;
  case 36..50:
    ret += "koenntest Du eigentlich einen Reisefuehrer herausbringen.";
    break;
  case 51..75:
    ret += "hast Du schon sehr viel gesehen.";
    break;
  default:
    ret += "besitzt Du eine hervorragende Ortskenntnis.";
  }
  return break_string(ret, 78, 0, 1);
}


nomask mixed QueryWizardRequirements(object player)
{
  // Diese Funktion gibt ein 2-elementiges Array zurueck, in dem im ersten
  // Element steht, ob der Spieler Seher werden kann (1) oder
  // nicht (0) und im zweiten Element steht, was genau ihm noch 
  // fehlt. 
  // Fehlercode ist ({-1, ""})

  // Die Umrechnungsfaktoren wurden einfach aus QueryLEP uebernommen; ggf.
  // kann man das einmal in Unterfunktionen auslagern. 

  mixed ret;
  string s; 
  int i,z,val; 

  ret = ({-1, "Hier ist etwas schief gelaufen. Bitte einen Erzmagier\n"
	    +"benachrichtigen.\n"});
  s = "";
  i = 0; 

  if(!player || !objectp(player))
    player=(this_player()?this_player():this_interactive());

  if(!player)
    return ({-1,""});

  DEBUG("Es geht um: %O\n", player);

  // Abenteuerpunkte
  DEBUG("Abenteuerpunkte: %d ("+REQ_QP+")\n", player->QueryProp(P_QP));
  if (player->QueryProp(P_QP) < REQ_QP) {
    s += sprintf(" * Dir fehlen noch mindestens %d Abenteuerpunkte.\n", 
		 REQ_QP - (int)player->QueryProp(P_QP));
    i--;
  }

  // Forscherpunkte
  z = 6 * (int)EPMASTER->QueryExplorationPoints(player);
  DEBUG("Forscherpunkte: %d ("+REQ_EP+")\n", z);
  if (z < REQ_EP) {
    s += sprintf(" * Du kennst Dich im "MUDNAME" noch nicht genug aus, "
		 +"genau genommen\n   musst Du Dir noch %s ansehen.\n", 
		 REQ_TEXT1[(z*10/REQ_EP)] );
    i--;
  }

  // Zaubertraenke
  z = 80 - (val = sizeof((int*)player->QueryProp(P_POTIONROOMS)));
  z = z*5 + ([0:100, 1:60, 2:30, 3:10])[val];
  DEBUG("Zaubertraenke: %d ("+REQ_P+")\n", z);
  if (z < REQ_P) {
    s += sprintf(" * Du musst noch einige Zaubertraenke (ca. %d) suchen.\n",
		 (REQ_P - z)/5 );
    i--;
  }

  // Erstkills
  z = (int)SCOREMASTER->QueryKillPoints(player);
  DEBUG("Erstkills: %d ("+REQ_K+")\n", z);
  if (z < REQ_K) {
        s += sprintf(" * Du hast noch nicht genuegend wuerdige Gegner erlegt, genau "
    	 +"genommen\n   musst Du noch %s wuerdige Monster toeten.\n",
    	 REQ_TEXT2[(z*10/REQ_K)] );;
    i--;
  }

  int minlevel = QueryLevel(REQ_LEP);

  // Restliche Stufenpunkte 
  DEBUG("Stufenpunkte: %d ("+REQ_LEP+")\n", player->QueryProp(P_LEP));
  if ((int)(player->QueryProp(P_LEP)) < REQ_LEP) {
    s += sprintf(" * Du musst mindestens %d Stufenpunkte, entspricht Stufe %d, "
        "erreichen.\n", REQ_LEP, minlevel);
    i--;
  }
  
  // Demnach mindestens REQ/100-Level 
  DEBUG("Level: %d ("+REQ_LEP/100+")\n", player->QueryProp(P_LEVEL));
  if ((int)player->QueryProp(P_LEVEL) < minlevel) {
    s += sprintf(" * Du musst mindestens Stufe %d erreichen.\n", minlevel);
    i--;
  }
  
  if(i<0) {
    ret = ({-1, 
	 sprintf("Du hast noch nicht alle Seher-Anforderungen erfuellt.\n"
		+"Im einzelnen fehlt Dir folgendes:\n\n%s\n"
		 +break_string("Falls Du Dir nun dennoch unsicher bist, "
  +"welche Anforderungen Du erfuellen musst, dann "
  +"schaue bei 'hilfe seher' und 'hilfe stufenpunkte' doch einfach noch "
  +"einmal nach. Sind dann "
  +"immer noch Dinge offen oder unklar, so sprich einfach einen "
				  +"der Erzmagier an.", 78,0,1),s) });
  } 
    
  if (i==0) {
    ret = ({1, break_string(
	       "Du hast alle Seher-Anforderungen erfuellt. Wende Dich doch "
	       +"einmal an Merlin und frage ihn, ob er Dich befoerdert.", 
	       78,0,1) });
  }

  return ret; 
}

nomask int QueryReadyForWiz(object player)
{
  mixed r;
  
  r = QueryWizardRequirements(player);
  
  if (!pointerp(r) && sizeof(r)!=2 && !intp(r[0]))
    return -1;

  return r[0];
}

nomask string QueryReadyForWizText(object player)
{
  mixed r;

  r = QueryWizardRequirements(player);

  if (!pointerp(r) && sizeof(r)!=2 && !stringp(r[1]))
    return "Hier ist etwas schief gegangen, bitte verstaendige einen "
      +"Erzmagier.";

  return r[1];
}

