FindPotion()
============

FUNKTION
--------
::

     varargs int FindPotion(string s);

DEFINIERT IN
------------
::

     /std/player/potion.c

ARGUMENTE
---------
::

     string s
       Ausgabetext. Wenn 0/Leerstring, wird Default verwendet.

BESCHREIBUNG
------------
::

     Diese Funktion gibt einem aufrufenden Spieler eventuell diesen ZT.

     Das aufrufende Spielerobjekt muss dafuer:
     * diesen ZT im Potionmaster in seiner Liste eingetragen haben
     * diesen ZT in der Liste der bekannten Traenke haben (durch
       Orakel also fuer ihn auch freigeschaltet)
     * darf keine Playerkills haben (P_KILLS)
     * darf nicht im Editiermodus sein
     * darf kein Geist sein (Ausnahme: Geisterschloss)

     Wenn alle Kriterien erfolgreich erfuellt sind, wird 's' oder 
     "Du findest einen Zaubertrank, den Du sofort trinkst." ausgegeben
     und dem Spieler ggf die Wahl der Attribute gegeben.

RUeCKGABEWERT
-------------
::

     0 bei Nichtvergabe, 1 bei erfolgreicher Vergabe.

BEISPIELE
---------
::

     string detail_papiere() {
       if (this_player()->FindPotion(
         break_string("Beim Rumwuehlen in den Papieren entdeckst Du einen "
                      "kleinen Zaubertrank, den Du sofort trinkst.", 78)))
         return "";  
         // Es muss ein String zurueckgegeben werden, da man sonst
         // die Fehlermeldung "Sowas siehst du hier nicht." bekommt
       else
         return "Die Papiere sind alle unbeschriftet.\n";
     }

HINWEISE
--------
::

    Zaubertraenke duerfen nicht in Raeumen eingetragen werden, die in der
    Parawelt liegen oder nur ueber Paraweltraeume zugaenglich sind.

    Der Auswahldialog fuer die Attribute wird an this_interactive() 
    ausgegeben, d.h. das Kommando, das zum Aufruf von FindPotion() fuehrt, 
    muss von dem Spielerobjekt eingegeben werden, an dem FindPotion()
    gerufen wird. Das bedeutet konkreter: Bei einem ZT, bei dem ein zweiter
    Spieler ein Kommando eingibt, damit ein anderer Spieler einen ZT findet, 
    funktioniert nicht.


SIEHE AUCH
----------
::

     Sonstiges: zaubertraenke, /secure/potionmaster.c, /room/orakel.c
     Verwandt:  AddKnownPotion(), RemoveKnownPotion(), InList()
     Props:     P_POTIONROOMS, P_KNOWN_POTIONROOMS
     Befehl:    traenke (fuer Magier zum Einschalten des Findens von ZTs)

19 Nov 2020 Arathorn

