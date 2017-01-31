QueryDefend()
=============

FUNKTION
--------
::

     int QueryDefend(string|string* dtyp, int|mapping spell, object enemy);

DEFINIERT IN
------------
::

     /std/armour/combat.c

ARGUMENTE
---------
::

     dtyp  - Schadenstypen der Angriffsart
     spell - 0       bei konventionellem Angriff,
             != 0    bei Angriff mit einem nichtphysischen Spell,
             mapping bei genaueren Angaben zur Wirkung
     enemy - Der angreifende Gegner

BESCHREIBUNG
------------
::

     Dies ist die zentrale Funktion einer Ruestung. Sie wird in jeder
     Kampfrunde aus /std/living/combat::Defend() fuer jede Ruestung aufgerufen,
     die der Spieler angezogen hat.

     Der Schutzwert von P_AC entfaltet seine Wirkung nur bei konventionellen
     Angriffen:
     * wenn 'spell' 0 ist (bei Aufruf aus der Defend heraus ausgeschlossen)
     * wenn 'spell' ein Mapping mit dem Flag SP_PHYSICAL_ATTACK != 0 UND
                    in 'dtyp' mindestens ein physischer Schaden enthalten ist

RUeCKGABEWERT
-------------
::

     Die Ruestungsstaerke in dieser Kampfrunde. Sie ermittelt sich als
     Zufallszahl zwischen 0 und P_AC, zuzueglich des Ergebnisses des
     DefendFunc()-Aufrufs.

BEMERKUNGEN
-----------
::

     Auch wenn man eine DefendFunc() benutzt, darf der Rueckgabewert
     insgesamt nicht groesser werden als der fuer den Ruestungstyp
     maximal zulaessige!

SIEHE AUCH
----------
::

     Ruestungen: P_ARMOUR_TYPE, P_NR_HANDS, P_ARMOURS, P_WORN
     Schutz:     P_AC, Defend(), DefendFunc
     Sonstiges:  P_EQUIP_TIME, P_LAST_USE, P_DAM_TYPE
     Verwandt:   QueryArmourByType(L), P_WEAPON, FilterClothing(),
                 FilterArmours()
     Resistenz:  P_RESISTANCE_STRENGTHS, CheckResistance(L)

28.Jul 2014 Gloinson

