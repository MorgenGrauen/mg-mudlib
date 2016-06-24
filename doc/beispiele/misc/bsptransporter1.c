inherit "std/transport";

/* wolke.c Ein Beispiel zur Programmierung eines Transporters */
/* von Rumata (mit kleinen Bugfixes von Wargon ;) */

#include <properties.h>
#include <language.h>

create()
{
  ::create();
  SetProp( P_NAME, "Woelkchen" );
  AddId( ({ "wolke", "woelkchen" }) );
  SetProp( P_GENDER, NEUTER );
  SetProp( P_INT_SHORT, "Auf einer kleinen Wolke" );
  SetProp( P_INT_LONG,
	  "Du sitzt auf einer kleinen Wolke, die zwischen Jofs und Rumatas\n"
	  +	"Arbeitszimmer hin- und herpendelt. Wenn sie niedrig fliegt, kannst\n"
	  +	"Du bestimmt von ihr heruntersteigen.\n"
	  );
  SetProp( P_SHORT, "Eine kleine Wolke" );
  SetProp( P_LONG,
	  "Diese suesse kleine Cumuluswolke schwebt zwischen Jofs und Rumatas\n"
	  +	"Arbeitszimmer hin und her. Vielleicht kannst Du auf sie draufsteigen.\n"
	  );
  SetProp(P_LIGHT, 1 );
  SetProp(P_TRANSPARENT,1);
  // Man soll auf(in) die Wolke und von ihr herunter schauen koennen.
    
  /*** Meldungen, die Transporter geben koennen ***/
  
  SetProp( P_DEPARTMSG, ({
    "Die Wolke steigt in die Luft.\n", 
    "Die Wolke fliegt davon.\n"
  }) );
  // Die erste Meldung ist fuer Leute auf der Wolke.
  // Die zweite fuer Leute in dem Raum, in dem die Wolke ankommt.
      
  SetProp( P_ARRIVEMSG, ({
    "Die Wolke naehert sich dem Boden von @@QueryArrived@@.\n",
    "Eine kleine Wolke schwebt herab.\n"
  }) );
  // Die erste Meldung ist fuer Leute auf der Wolke.
  // Die zweite fuer Leute in dem Raum, aus dem die Wolke abreist.
    
  SetProp( P_ENTERMSG, ({
    "steigt auf die Wolke",
    "kommt auf die Wolke"
  }) );
  // Die erste Meldung ist fuer den Raum, aus dem der Spieler kommt.
  // (=Raum). Die zweite Meldung ist fuer Spieler in dem Raum, in
  // den der Spieler geht (=Transporter).
	
  SetProp( P_LEAVEMSG, ({
    "steigt von der Wolke",
    "steigt von der Wolke"
  }) );
  // Die erste Meldung ist fuer den Raum, aus dem der Spieler kommt.
  // (=Transporter). Die zweite Meldung ist fuer Spieler in dem Raum,
  // in den der Spieler geht (=Raum).
      
  SetProp( P_LEAVEFAIL, "Die Wolke schwebt viel zu hoch" );
  // Meldung, die kommt, wenn ein Spieler den Transporter verlassen
  // will, aber dieser an keinem Raum angelegt hat.
  // Ist der Parameter ein String (so wie hier), so bekommt nur
  // der betreffende Spieler die Meldung, wenn sie ein Array aus
  // 2 Elementen ist, bekommt der Spieler die erste und alle
  // anderen Spieler im Transporter die zweite (mit dem Namen
  // des Spielers davor).
  // Moeglich waere also auch:
  /*
     SetProp( P_LEAVEFAIL, ({
     "Die Wolke schwebt viel zu hoch zum Herunterspringen",
     "faellt fast von der Wolke"
     }) );
  */

  SetProp( P_ENTERFAIL, "Es passen nur 5 Passagiere auf die Wolke.\n" );
  // Diese Meldung bekommt der Spieler, wenn er versucht, einen
  // vollen Transporter zu betreten. Hier ist nur eine Meldung
  // (an den Spieler) vorgesehen.

  SetProp( P_MAX_PASSENGERS, 5 );
  // Maximale Anzahl der Spieler, die sich im Transporter befinden
  // koennen. Fuer Gegenstaende gibt es keine Beschraenkung.

  /* Verben */
  AddCmd( ({ "steig", "steige", "spring", "springe" }), "steige" );

  /* Kurs */
  AddRoute( "players/rumata/workroom", 30, 20, "Rumatas Arbeitszimmer" );
  // Hier wird der Raum /players/rumata/workroom als Anlegeplatz
  // in den Kurs eingefuegt. Der Transporter verweilt 30sek in diesem
  // Raum und faeht dann 20sek lang, bis er den naechten Punkt
  // erreicht. Danach kann man noch den Code angeben, der bei
  // QueryArrived zurueckgegeben wird, setzen. Wird kein Code
  // gesetzt, wo wird dort ein Leerstring zurueckgegeben. 0 wird
  // dann zurueckgegeben, wenn der Transporter an keinem Raum angelegt
  // hat.

  AddMsg( "Die Wolke treibt nun im Wind Richtung Jofs Arbeitszimmer.\n", 30 );
  // Hier wird eine Meldung ausgegeben, und der Transporter reist 30sek lang
  // weiter.

  AddMsg( "Die Wolke beginnt zu sinken.\n", 10 );
  AddRoute( "players/jof/workroom", 30, 20, "Jofs Arbeitszimmer" );
  AddMsg( "Die Wolke treibt nun im Wind Richtung Rumatas Arbietszimmer.\n", 30 );
  AddMsg( "Die Wolke beginnt zu sinken.\n", 10 );
  // Nach dem Letzten Haltepunkt wird der Kurs wieder von vorn
  // befahren.
 	
  Start();
  // Lasse den Transporter am ersten dieser Haltepunkte starten.
}

steige(str)
{
  string arg, arg2;
  
  if (sscanf( str, "auf %s", arg ) > 0 && id(old_explode(arg," ")[0]))
    return Enter();
  // Wenn sicher ist, dass der Spieler die Wolke BEsteigen will,
  // kann man mit return Enter() alle weiteren Tests durchfuehren.

  if (sscanf( str, "von %s", arg ) > 0 && id(old_explode(arg, " ")[0]))
    return Leave();
  // Das selbe gilt fuer das ABsteigen und Leave().
  // Verben sollten nach Enter() oder Leave() keine weiteren Befehle
  // enthalten.
}


