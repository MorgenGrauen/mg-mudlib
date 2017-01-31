P_ATTACK_BUSY
=============

NAME
----
::

    P_ATTACK_BUSY                 "attack_busy"                 

DEFINIERT IN
------------
::

    /sys/living/combat.h

BESCHREIBUNG
------------
::

    Ueber diese Property kann festgestellt werden, ob ein Spieler noch 
    Spezialwaffen (zB Flammenkugel) einsetzen kann.

    

    Ist der Wert bei Abfrage ungleich 0, dann darf der Spieler in dieser
    Runde keine Aktion mehr durchfuehren. Mit SetProp(P_ATTACK_BUSY, 1)
    wird eine Aktion verbraucht.

    Intern wird relativ fein gerechnet, ein SetProp(P_ATTACK_BUSY, x)
    wird in das Abziehen von x*100 Punkten umgerechnet. Der Wert freier
    Aktionen pro Runde berechnet sich wie folgt:

    

    Spieler: 100 + QuerySkillAttribute(SA_SPEED)
    Seher:   Spieler + 200 + QueryProp(P_LEVEL)

    Das Maximum liegt bei 500.
    Damit betraegt die Anzahl der moeglichen Aktionen pro Runde: Wert/100,
    also maximal 5 Aktionen pro Runde.

    Zaubersprueche zaehlen im Normalfall auch als eine Aktion.

BEMERKUNGEN
-----------
::

    Benutzt man P_ATTACK_BUSY fuer eine sich nicht sofort verbrauchende
    Sache, kann ein Seher dieses Objekt im Normalfall dreimal pro Runde
    benutzen. Wenn ungewollt, muss das ueber einen Zeitmarker selbst
    verhindert werden.

    

BEISPIELE
---------
::

    (Code eines Objektes ist in
     /doc/beispiele/testobjekte/attack_busy_sensitive_testobj.c)
    // einfacher Test auf ATTACK_BUSY und anschliessendes Setzen
    if (this_player()->QueryProp(P_ATTACK_BUSY)) {
      write("Du hast dafuer momentan einfach nicht mehr die Puste.\n");
      return 1;
    }
    this_player()->SetProp(P_ATTACK_BUSY, 1);

7. Mar 2011 Gloinson

