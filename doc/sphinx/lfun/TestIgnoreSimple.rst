TestIgnoreSimple()
==================

FUNKTION
--------
::

     public int TestIgnoreSimple(string *arg)

DEFINIERT IN
------------
::

     /std/player/comm.c

ARGUMENTE
---------
::

     arg
         Liste von Strings, die getestet werden sollen

BESCHREIBUNG
------------
::

     TestIgnoreSimple() prueft, ob der Spieler min. einen der uebergebenen
     Eintraege auf seiner Ignoriereliste hat.
     Falls man mehrere Eintraege pruefen muss/moechte, ist es schneller, alle
     Eintraege in einem zu uebergeben anstatt fuer jeden einzeln 
     TestIgnoreSimple() aufzurufen.

RUeCKGABEWERT
-------------
::

     1, falls es mindestens eine Uebereinstimmungen von arg und der
     Ignoriere-Liste des Spielers gibt.
     0, sonst.

BEISPIEL
--------
::

     if (!this_player()->TestIgnoreSimple(({"andy"})))
       tell_object(this_player(), "Andy teilt Dir mit: Hallo!\n");

     // Beispiel fuer eine Ignore-Check fuer Aktion (kratzen) fuer einen
     // Spieler (this_player()) an einem anderen Spieler (target)
     if (!target->TestIgnoreSimple(getuid(this_player()),
                             getuid(this_player())+".kratz",
                             getuid(this_player())+".kratze",
                             ".kratz", ".kratze"}))) {
       tell_object(target, this_player()->Name()+" kratzt dich.\n");
       tell_object(this_player(), "Du kratzt "+target->Name()+".\n");
     } else
       tell_object(this_player(), target->Name()+" ignoriert dich.\n");

     // allumfassender Ignorier-Check in einer Gilde (Klerus) auf
     // eine Aktion (kurieren) fuer einen bestimmten Spieler (den caster)
     // an einem zu kurierenden Spieler (target)
     if (target->TestIgnoreSimple(({getuid(caster),
                              getuid(caster)+".kuriere",
                              getuid(caster)+".kuriere.klerus",
                              ".kuriere",
                              ".kuriere.klerus"})))
       tell_object(caster, break_string(
         target->Name()+" ignoriert deinen Versuch.", 78));

SIEHE AUCH
----------
::

     P_IGNORE, AddIgnore, RemoveIgnore, TestIgnore, /std/player/comm.c

26.04.2014 Zesstra

