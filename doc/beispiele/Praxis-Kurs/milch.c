// (c) by Padreic (Padreic@mg.mud.de)

// Anmerkung: Heutzutage gibt es fuer Lebensmittel (bzw. alle Dinge, die
// man essen oder trinken kann) ein Standardobjekt namens /std/food,
// welches viele Funktionalitaet mitbringt, die man ansonsten selber
// muehsam basteln muss. Die Verwendung ist dringend empfohlen.

#include <properties.h> // wieder unsere allgemeinen Properties

inherit "/std/thing";

// globale Variable, die angibt, wie voll die Flasche ist. Es sind 5
// Portionen enthalten.
int menge = 5;

protected void create()
{
   ::create();
   // Objekt ueber seine Properties beschreiben...
   // P_SHORT und P_LONG werden direkt als Funktion implementiert (s.u.)
   SetProp(P_NAME, "Milchflasche");
   SetProp(P_GENDER, FEMALE);
   SetProp(P_VALUE, 80);
   SetProp(P_WEIGHT, 1000);
   AddId(({"flasche", "milchflasche"}));
   AddCmd(({"trink", "trinke"}), "_trinken");
}

// Anstelle von P_SHORT Kurzbeschreibung ueber Funktion short().
// Dies ist nicht immer moeglich, da nicht zwangslaeufig zu jeder Propertie
// eine gleichnamige Funktion existiert.
// Es ist allerdings noch sog. Querymethoden (siehe dazu <man QueryProp> und
// <man Query> bei Bedarf).
public string short()
{
   string str;
   switch(menge) {
     case 1: str="Eine fast leere Milchflasche.\n"; break;
     case 2: str="Eine bald leere Milchflasche.\n"; break;
     case 3: str="Eine halbvolle Milchflasche.\n";  break;
     case 4: str="Eine fast volle Milchflasche.\n"; break;
     case 5: str="Eine volle Milchflasche.\n";      break;
     default: str="Eine leere Milchflasche.\n";
   }
   return str;
}

public string long()
{
   if (menge>1)
      return "Eine Flasche mit leckerer Vollmilch.\n";
   else
      return "Die Milchflasche ist bald leider schon alle, Du solltest "
             "Dich vielleicht mal\nnach Nachschub umsehn.\n";
}

public int _trinken(string str)
{
   notify_fail("Syntax: trinke aus milchflasche\n");

   // Falls die ersten vier Zeichen in dem uebergebenen String ungleich
   // "aus " sind, dann fuehlt sich das Objekt nicht angesprochen, kann das
   // Verb also nicht komplett abarbeiten und gibt deshalb eine 0 zurueck.
   if (str[0..3]!="aus ") return 0;

   // Ich benutze nun alles ab dem vierten Zeichen und pruefe, ob sich das
   // Objekt davon angesprochen fuehlt. Hierzu dient die Funktion id().
   // Grosser Vorteil: Uebersichtlich und spaeteres Ergaenzen von Ids bzw.
   // Adjektiven geht sehr einfach und erspart lange if()-Abfragen.
   if ( !id(str[4..]) ) return 0;

   if (menge<=0) {
     write("Die Milchflasche ist leider schon leer :(.\n");
   }
   else {
     // drink_soft() ist eine Funktion, die in allen Lebewesen im
     // Morgengrauen definiert ist und ueber die man sowohl Abfragen als
     // auch hochsetzen von P_DRINK vornehmen sollte.
     if (this_player()->drink_soft(10)) {

       // heal_self() ist auch in allen Lebewesen definiert und heilt bei
       // dem Lebewesen Lebens- und Konzentrationspunkte.
       this_player()->heal_self(10);
       write("Genuesslich trinkst Du einen Schluck Milch.\n");
       say(this_player()->Name(WER)+" trinkt einen Schluck Vollmilch,\n");
       menge--;
     }
     else write("Soviel kannst Du leider nicht mehr trinken.\n");
   }
   return 1;
}
