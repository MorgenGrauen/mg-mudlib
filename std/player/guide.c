// MorgenGrauen MUDlib
// 
// player/guide.c -- newbie guide handling 
//
// $Id: guide.c 7391 2010-01-25 22:52:51Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#include <config.h>
#define NEED_PROTOTYPES
#include <player/user_filter.h> // fuer is_active_guide()
#include <properties.h>
#include <defines.h>
#include <thing/properties.h>

/* Funktion, die die Guide-Kommandos aktiviert */
void add_guide_commands()
{
	add_action("CiceroneCmd","cicerone");
	// Sollte eigentlich an derselben Stelle moeglich sein.
}

/* Gibt die Meldung beim Aendern aus*/
protected int NewbieChangeMsg() {
    int cic=QueryProp(P_NEWBIE_GUIDE);
    // begrenzen auf 1 Tag, falls jemand da Schrott reingschrieben hat.
    if (cic > 86400) {
	cic=86400;
	SetProp(P_NEWBIE_GUIDE,cic);
    }
    if (cic<=0) {
	write("Du bist jetzt kein Cicerone mehr.\n");
    }
    else if (cic < 60) {
	write("Du bist jetzt ein Cicerone.\n");
    }
    else {
	write(break_string(
	    "Du bist jetzt ein Cicerone, allerdings nur in den Zeiten, "
	    "in denen Du weniger als " + cic/60
	    + ((cic/60)<2 ? " Minute ":" Minuten ")
	    + "idle bist.\n",78));
    }
    return 1;
}

/* Gibt die Statusmeldung aus */
protected int NewbieStatusMsg() {
  int cic=QueryProp(P_NEWBIE_GUIDE);
  // begrenzen auf 1 Tag, falls jemand da Schrott reingschrieben hat.
  if (cic > 86400) {
      cic=86400;
      SetProp(P_NEWBIE_GUIDE,cic);
  }

  if (cic <= 0)
    write ("Du bist kein Cicerone.\n");
  else if (cic < 60)
    write ("Du stehst Neuspielern als Cicerone zur Verfuegung.\n");
  else {
    write(break_string(
	    "Du stehst Neuspielern als Cicerone zur Verfuegung, allerdings "
	    "nur in den Zeiten, in denen Du weniger als " + cic/60
	    + ((cic/60)<2 ? " Minute ":" Minuten ")
	    + "idle bist.\n",78));
  }
  return 1;
}

/* Fuehrt das eigentliche Kommando aus*/

int CiceroneCmd(string str)
{
	if (QueryProp(P_LEVEL)<20)
	{
		write(break_string("Du solltest erst noch ein wenig "
					"Erfahrung sammeln, bevor Du Dich "
				       "als Cicerone zur Verfuegung stellst.",78));
		return 1;
	}
	// Idlezeit uebergeben?
	int idle=to_int(str);
	// max. einen Tag (1440 min) zulassen.
	if (idle < 0) idle=0;
	else if (idle > 1440) idle=1440;

	if (!str) {
	    return NewbieStatusMsg();
	}
	else if (str=="status") {
	    return NewbieStatusMsg();
	}
	// "ein" schaltet einfach generell ein, hierbei steht 1 in der Prop
	// fuer "permanent ein".
	else if (str=="ein") {
	    SetProp(P_NEWBIE_GUIDE,1);
	    return NewbieChangeMsg();
	}
	// "aus" oder "0" deaktiviert.
	else if (str=="aus") {
	    SetProp(P_NEWBIE_GUIDE,0);
	    return NewbieChangeMsg();
	}
	// wenn Zahl uebergeben ist, die groesser 0 und kleiner 1440 ist
	// (s.o.), wird es als Anzahl an Idle-Minuten aufgefasst, ab der man
	// ausgeblendet werden will.
	else if (idle) {
	    SetProp(P_NEWBIE_GUIDE, idle*60); // als Sekunden speichern.
	    return NewbieChangeMsg();
	}
	write(break_string(
	      "cicerone ein  - Du bist Cicerone\n"
	      "cicerone aus  - Du bist kein Cicerone\n"
	      "cicerone      - Status anzeigen\n"
	      +break_string(
		"Du bist Cicerone, aber wenn Du laenger als <zahl> Minuten "
		"idle bist, wirst Du automatisch ausgeblendet, bis Du wieder "
		"endidelt bist.",
		76,"cicerone zahl - ",BS_INDENT_ONCE),
	      78,"Syntaxhilfe:",BS_PREPEND_INDENT|BS_LEAVE_MY_LFS));

	return 1;
}

protected string IstSindMsg(string* namen)
{
	if (sizeof(namen)==1)
		return "ist davon "+namen[0];
	else
		return "sind davon "+CountUp(namen);
}

void NewbieIntroMsg()
{
	object* cicerones;
	string* namen;
	string restext;

	// Nur bis Level 5 wird etwas ausgegeben.
	if (QueryProp(P_LEVEL)>5) return;
	
	// is_active_guide() ist in /std/user_filter.c, welches vom
	// Spielerobjekt geerbt wird und damit zur Verfuegung steht.
	cicerones=filter(users(),#'is_active_guide);
	// uid verwenden, da sonst kleine Spieler einen getarnten 
	// "Riesen" oder aehnliches anstprechen.
	namen=map(cicerones,function string (object o) 
	    { return(capitalize(geteuid(o))); } );

	if (namen && sizeof(namen)>0)
	{
		restext="\nEs gibt einige nette Spieler, die bereit sind, Dich "
				"auf Deinen ersten Schritten im "MUDNAME
				" zu begleiten. \n\nDerzeit "
				+IstSindMsg(namen)+" eingeloggt. Du kannst "
				"einen oder eine von Ihnen ansprechen, "
				"indem Du z.B. einfach \n"
				"  'teile "+
				lower_case(namen[random(sizeof(namen))])+
				" mit Hallo ich bin neu hier, kannst Du "
				"mir bitte helfen?'\n"
				"eintippst. Nur keine Scheu, diese Spieler "
				"haben sich freiwillig dazu bereiterklaert!\n"
				"\nDu kannst Dir diese Spieler jederzeit "
				"mit 'kwer cicerones' anzeigen lassen.\n\n";
		write(break_string(restext,78,"*  ",BS_LEAVE_MY_LFS));
	}
	else
	{
		// Weia, kein Newbie-Guide da. Lieber erstmal nix tun,
		// bis uns was besseres einfaellt.
	}
	return;
}




