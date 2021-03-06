P_NO_ATTACK
===========

NAME
----
::

     P_NO_ATTACK "no_attack"

DEFINIERT IN
------------
::

     <living/combat.h>

BESCHREIBUNG
------------
::

     Wenn ein NPC nicht angreifbar sein soll (weil er zum Beispiel in einer
     Gilde oder einer Quest Informationen vermittelt oder aehnlichen), sollte
     man diese Property auf einen Wert ungleich Null setzen. Sie wird immer
     abgefragt, wenn ermittelt wird, ob ein Lebewesen prinzipiell angreifbar
     ist. D.h. auch, dass nach Abfragen von P_NO_ATTACK _nicht_ immer ein
     Kampf gestartet wird und dass man dabei _nicht_ im Kampf sein muss!

     Gibt man hier einen String an (mit einem Satzzeichen und "\n" abge-
     schlossen), wird dieser bei direkten Angriffen ausgegeben. Bei anderen
     Datentypen wird eine Defaultmeldung ausgegeben. Die Defaultmeldung
     lautet: "<Name> laesst sich nicht angreifen!\n"

     Mit direkten Angriffen sind 'toete <name>' und Angriffszauber gemeint
     (bzw. alles, was living/life::Kill(), spellbook::TryAttackSpell(),
     spellbook::TryDefaultAttackSpell() und spellbook::FindEnemyVictim()
     aufruft).

ACHTUNG
-------
::

  1) Zum Thema QueryMethoden auf P_NO_ATTACK
     Grundsaetzlich legt man entweder eine Query-Methode auf P_NO_ATTACK:
        Set(P_NO_ATTACK, #'my_no_attack, F_QUERY_METHOD);
     oder definiert eine Funktion _query_no_attack() im NPC.

     Wie muss nun eine solche Funktion aussehen? Z.B.:

     

     int|string my_no_attack() {
       if (!objectp(this_player())) return 0;
       if (opfer==getuid(this_player()) || this_player()==this_object())
         return(0);
       return(1); //nicht angreifbar
     }

     Diese Funktion macht den NPC nun nur fuer den Spieler 'opfer' angreifbar.
     Stattdessen kann natuerlich auch jede andere Bedingung genutzt werden.

     Aber warum die zweite Bedingung, this_player()==this_object()?
     Warum sollte der NPC sich selber angreifen duerfen?

     Das liegt an folgenden 2 Dingen:

     1. Kaempfer kriegen bei eingeschaltetem Fokus Probleme, wenn man das 
     nicht macht. Das liegt an folgendem: Wenn der NPC angreift, ruft er 
     natuerlich Defend() im Spieler auf. Dieses schaut nach, ob der Spieler 
     den Skill SK_MAGICAL_DEFENSE hat. Dieser ist bei Kaempfern das Parieren.
     Dieses schaut nach, ob der Fokus aktiv ist, wenn ja, wird dem 
     ge'fokus'te Gegner besonders gut ausgewichen. Zu diesem Zweck wird die 
     Liste der Feind im Raum erstellt mit PresentEnemies() abgerufen. Dieses 
     fragt aber in allen (potentiellen) Gegnern P_NO_ATTACK ab und beendet 
     den Kampf mit allen Gegnern, die nicht angreifbar sind. Bei dieser 
     Abfrage ist jedoch TP==NPC, weil der ja angreift. Wenn er nun 1 
     zurueckgibt, wird der Kampf an der Stelle beendet. 

     2. Wenn der NPC den Spieler angreift, wird im Spieler InsertEnemy(NPC)
     aufgerufen. Auch diesem Fall findet die Abfrage von P_NO_ATTACK statt, 
     da InsertEnemy() ja erstmal rausfinden muss, ob der Gegner angreifbar 
     ist, bevor er in die Feindliste eingetragen wird. Da der NPC den 
     Angriff beginnt, ist TP der NPC. Wenn die Query-Methode auf P_NO_ATTACK
     hier abbricht, wird der NPC nicht in die Feindliste des Spielers 
     eingetragen. Dann bekaempft der NPC den Spieler, aber der Spieler nicht
     den NPC.


  2) P_NO_ATTACK des NPC wird z.B. beim Kampf eines Kaempfers mit dem NPC 
     pro Kampfrunde um die 10mal abgerufen. Wenn der Kaempfer nur eine 
     Attacke macht. Wenn er noch Sonderattacken machen, Spells ausfuehrt, 
     etc. wird das noch mehr. D.h. was auch immer ihr in der Query-Methode 
     im NPC macht: 
     Es sollte schnell sein, jeder Tick an Rechenzeit zaehlt hier xfach!

LETZTE AENDERUNG
----------------
::

09.11.2015, Arathorn

