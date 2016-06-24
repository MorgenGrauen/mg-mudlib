// (c) by Padreic (Padreic@mg.mud.de)

#include <properties.h> // wieder unsere allgemeinen Propertys
#include <moving.h>     // einige Defines zum Bewegen von Objekten (s.u.)

inherit "/std/thing";

protected void create()
{
  ::create();
   // Objekt ueber seine Properties beschreiben...
   SetProp(P_SHORT, "Ein Rubbellos");
   SetProp(P_LONG,
    "Du kannst dieses Los aufrubbeln um zu sehen, ob Du einen grossen "
    "Schatz\ngewonnen hast.\n");
   SetProp(P_NAME, "Rubbellos");
   SetProp(P_ARTICLE, 1); // ist eigentlich bereits Defaultwert
   SetProp(P_GENDER, NEUTER);
   SetProp(P_VALUE, 1000);
   SetProp(P_WEIGHT, 500);
   SetProp(P_INFO, "Deine Gewinnchancen bei diesen Losen sind nicht "
                   "sonderlich gut.\n");
   SetProp(P_MAGIC, 0); // (einfach nur, um mal alle Properties zu benutzen)
   AddId("los"); // noch eine id angeben...

   // Mit Hilfe dieser neuen wichtigen Funktion ist es moeglich, Befehle zu
   // definieren, ueber die man irgendetwas ausloesen koennen soll:
   // Und zwar wird sobald ein Spieler den Befehl rubble oder rubbel
   // benutzt, die Funktion _rubbeln() hier im Objekt aufgerufen. Da diese
   // Funktion bisher ja noch nicht definiert ist, muss diese natuerlich
   // auch noch programmiert werden.
   AddCmd(({"rubble", "rubbel"}), "_rubbeln");
}

// Diese Funktion sieht gleich um einiges komplexer aus als die Funktionen
// create(), init() und reset(), die bisher benutzt wurden.
// Das static am Anfang erklaert nur, dass diese Funktion nicht von aussen,
// sondern nur von innerhalb des Objektes aufgerufen werden kann.
// Das "int" anstelle des "void" bedeutet, dass die Funktionen ein Ergebnis
// zurueckliefert. Da diese Funktion in Verbindung mit AddCmd() benutzt
// wird, hat dieser Rueckgabewert auch eine besondere Bedeutung:
//   Eine 1 bedeutet, dass der Befehl fertig abgearbeitet wurde.
//   Eine 0 bedeutet, das Objekt konnte mit dem Befehl nichts anfangen, der
//    Gamedriver muss also noch in weiteren Objekten anfragen, die der
//    Spieler bei sich traegt.
// Der String 'str', den die Funktion als Parameter uebergeben bekommt,
// enthaelt das, was der Spieler ausser dem eigentlichen Verb eingegeben
// hat (Woerter wie 'und', 'ein', 'der', 'die', 'das' ... werden hierbei
//  zuvor herausgefiltert)
// Bsp.:   rubbel los      ->  _rubbeln("los")
//         rubbel katze    ->  _rubbeln("katze")
//         rubbel los auf  ->  _rubbeln("los auf")
public int _rubbeln(string str)
{
   // Die Funktion notify_fail() ist wieder eine Funktion des Gamedrivers:
   // Und zwar ist es moeglich hier eine Meldung anzugeben, die anstelle
   // eines "Wie bitte?" kommt, falls kein Objekt den Befehl richtig
   // abgearbeitet hat.
   notify_fail("Syntax: rubbel los auf\n"); // eigentlich efun::notify_fail()

   // Wenn der uebergebene String ungleich "los auf" ist, dann fuehlt sich
   // das Objekt nicht angesprochen, kann das Verb also nicht komplett
   // abarbeiten und gibt deshalb eine 0 zurueck.
   if (str!="los auf") return 0;

   // Auch die Funktion random() ist wieder eine Funktion des Gamedriver:
   // Und zwar liefert sie eine ganzzahlige Zufallszahl zwischen 0 und n-1
   //  (wobei n die uebergebene Zahl ist). In diesem Fall also zwischen
   // 0 und 99.
   if (random(100)<92) { // sollte diese Zahl < 92 sein, dann tue Folgendes:
     write("Du rubbelst das Los auf. Leider war es jedoch nur eine Niete.\n"
          +"Veraergert laesst Du das Los deshalb fallen.\n");
   }
   else { // sollte die Zahl nicht < 92 gewesen sein, dann tue Folgendes:
     object ob; // bereitstellen einer Hilfsvariablen

     write("Du rubbelst das Los auf und strahlst vor Freude. Es war der "
          +"absolute Hauptgewinn.\nSofort kommt ein Bote herein und "
          +"ueberreicht Dir Deinen Gewinn.\n");

     // Mit clone_object() kann man ein beliebiges Objekt Clonen, indem man
     // einfach den entsprechenden Filenamen als Parameter uebergibt.
     ob=clone_object("/doc/beispiele/Praxis-Kurs/juwelen");

     // Nun kommen gleich 3 neue Dinge auf einmal...
     // Mit -> wird eine Funktion in einem anderen Objekt aufgerufen
     //  (alternativ auch: call_other(ob,move,this_player(), M_GET); )
     // Die Funktion, die hierbei aufgerufen werden soll, heisst move().
     // Diese Funktion ist in allen Objekten definiert, die /std/thing erben.
     // Somit koennen wir also fest davon ausgehen, dass sie auch in unseren
     // Juwelen vorhanden ist.
     // this_player() ist eine sehr wichtige Gamedriver-Funktion, da sie
     // uns das Lebewesen zurueckliefert, das gerade aktiv durch einen Befehl
     // den aktuellen Funktionsaufruf gestartet hat.
     // M_GET ist lediglich ein Define, das in moving.h definiert ist.
     // Naeheres hierzu findet man auch in <man move>.
     ob->move(this_player(), M_GET);
     // ACHTUNG: um dieses Beispiel simpel zu halten, wird hier darauf
     // verzichtet, zu pruefen, ob diese Bewegung ueberhaupt funktioniert
     // hat. Normalerweise muss man in diesem Fall (misslungene Bewegung)
     // eine Fehlerbehandlung durchfuehren (z.B. ob wieder entfernen,
     // Meldung an den Spieler ausgeben).
   }

   // Die Funktion say() gibt an alle Spieler im Raum des aktiven Spielers
   // eine Nachricht aus, aber nicht an den betroffenen Spieler selbst!
   // Die Funktion Name(), die hier im Spieler aufgerufen wird, wertet
   // dessen Propertie P_NAME aus und dekliniert den Namen im angegebenen
   // Fall.
   say(this_player()->Name(WER)+" rubbelt ein Los auf.\n");

   // Und nun soll sich das Objekt selbst entfernen (zerstoeren).
   remove();

   // Da der Befehl ja komplett abgearbeitet wurde, geben wir eine 1
   // zurueck.
   return 1;
}
