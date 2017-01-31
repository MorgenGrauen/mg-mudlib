P_M_HEALTH_MOD
==============

NAME
----
::

     P_M_HEALTH_MOD                  "magic_health_modifier"

DEFINIERT IN
------------
::

     /sys/living/attributes.h

BESCHREIBUNG
------------
::

     Mapping, mit dem die maximalen Lebenspunkte und Magiepunkte eines 
     Spielers veraendert werden, der diese Ruestung/Waffe traegt/benutzt. Im 
     Gegensatz zu P_M_ATTR_MOD erfolgt hier jedoch keine Blockade.

     Zu beachten: P_M_HEALTH_MOD kann problemlos durch ein SetProp() gesetzt 
     werden, es wird nur beruecksichtigt, wenn die Ruestung/Waffe 
     getragen/benutzt wird. Beim tragen/ausziehen/zuecken/wegstecken wird im 
     Spieler automatisch UpdateAttributes() aufgerufen.

     Fuer Krankheiten etc. verwendet man besser die Property P_X_HEALTH_MOD.

     Bitte beachten: Die positiven Modifier werden nicht mehr 1:1 auf die
     Lebens- und Magiepunkte draufaddiert. Stattdessen wird nur noch ein 
     gewisser Teil dort hinzuaddiert, der mit groesserer Menge von Punkten
     zunimmt, und im unteren Bereich grob dem Wert entspricht. Das 
     theoretische Maximum ist insgesamt 149.

BEMERKUNGEN
-----------
::

     Die Werte sollten moeglichst nicht dynamisch geaendert werden.
     Wenn doch, muss mit TestLimitViolation() am Spieler auf Validitaet 
     geprueft werden und mit UpdateAttributes() an ihm ggf. upgedatet.

BEISPIEL
--------
::

     SetProp(P_M_HEALTH_MOD,([P_HP:5,P_SP:-5]));
     // Dem Spieler, der das Objekt benutzt, wird P_MAX_HP um 5 erhoeht und
     // P_MAX_SP um 5 erniedrigt.

SIEHE AUCH
----------
::

     P_X_HEALTH_MOD, P_X_ATTR_MOD, P_M_ATTR_MOD, balance

LETZTE AeNDERUNG
----------------
::

    Fre,11.05.2007, 00:20 von Humni

