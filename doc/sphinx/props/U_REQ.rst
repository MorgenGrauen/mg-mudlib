U_REQ
=====

NAME
----
::

    U_REQ                         "u_req"

DEFINIERT IN
------------
::

    /sys/unit.h

BESCHREIBUNG
------------
::

     Die Prop kann in Unitobjekten gesetzt werden.
     Sie gibt die Anzahl der Einheiten an, die an der Unit manipuliert werden
     sollen, falls mit weniger als P_AMOUNT umgegegangen werden soll.

     

     Die Prop wird automatisch gesetzt, wenn id() an einem Unitobjekt gerufen
     wird und die ID grundsaetzlich zutreffend ist, die aus der ID ermittelte
     Zahl aber kleiner als P_AMOUNT ist.
     Sie kann auch manuell mittel SetProp() (aber nicht Set()) gesetzt werden.

     U_REQ wird beim Bewegen und Zerstoeren, bei Ermittlung von Wert und
     Gewicht beruecksichtigt.

     U_REQ wird vom Unitobjekt automatisch wieder auf P_AMOUNT gesetzt, wenn
     sich query_verb() oder debug_info(DINFO_EVAL_NUMBER) veraendert haben.
     (DINFO_EVAL_NUMBER ist eine Zahl, die sich jedesmal erhoeht, wenn der
      Driver eine neue Berechnung/Ausfuehrung beginnt. Diese Nummer wird fuer
      jeden vom driver initiierten Aufruf von LPC-Code erhoeht, z.B. bei jedem
      Kommando, call_out, heart_beat etc. Details s. debug_info().)

     Ebenso wird U_REQ bei der Vereinigung mit einem anderen (gleichen)
     Objekt auf P_AMOUNT des vereinigten Objektes gesetzt.

BUGS
----
::

    Viele. Dies ist ein uebler Hack. Geht aber nicht ohne.
    Diese Prop war endlos lang gar nicht dokumentiert. Hier beschrieben ist
    das Verhalten, was zur Zeit vorliegt. Dies mag unterschiedlich zu dem
    irgendwann intendierten sein.

BEISPIELE
---------
::

    object o = clone_object("unitobjekt");
    o->SetProp(P_AMOUNT, 100);   // ab jetzt hat o 100 Einheiten.
    o->move(npc, M_GET);         // ob mit 100 Einheiten wird bewegt
    o->SetProp(U_REQ, 50);
    o->move(anderernpc, M_GIVE); // 50 Einheiten werden bewegt, 50 verbleiben
    (Technisch: das Objekt wird auf 50 Einheiten geaendert, bewegt und in der
     alten Umgebung wird ein neues Objekt mit 50 Einheiten erzeugt.)

    o->SetProp(U_REQ, 42);
    o->remove(1);               // 42 Einheiten von den 50 werden zerstoert.
    (Technisch: P_AMOUNT wird einfach um U_REQ reduziert.)

    # gib 18 muenzen an blupp
    Hierbei wird ob->id("18 muenzen") gerufen, was U_REQ im Geldobjekt auf 18
    setzt. Bei der Bewegung bekommt blupp daher das Objekt mit P_AMOUNT==18
    und der Rest wird im Abgebenden neu erzeugt.
    # gib geld an flubbel
    Das U_REQ aus dem verherigen Kommando ist jetzt nicht mehr gueltig. Zwar
    ist es das gleiche Kommandoverb, aber DINFO_EVAL_NUMBER ist jetzt
    anders.

ZULETZT GEAeNDERT
-----------------
::

16.01.2015, Zesstra

