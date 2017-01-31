P_PARRY
=======

NAME
----
::

     P_PARRY "parry"

DEFINIERT IN
------------
::

     <combat.h>

BESCHREIBUNG
------------
::

     Diese Property legt fest, inwiefern eine Waffe als Parierwaffe
     genutzt werden kann. Moegliche Werte:

         PARRY_NOT     Eine reine Angriffswaffe ohne Parierfunktion.

         PARRY_TOO     Eine kombinierte Angriffs- und Parierwaffe.

         PARRY_ONLY    Eine reine Parierwaffe. Diese kann zusaetzlich
                       zu einer normalen Waffe gezueckt werden.

     Man sollte nur die in <combat.h> definierten Konstanten verwenden.

BEMERKUNGEN
-----------
::

     Durch diese Propertie laesst sich _kein_ Parade-Bonus fuer Trves 
     setzen! Alle Gilden haben etwas davon. Vor Verwendung bitte mit
     der Objekt-Balance absprechen.

SIEHE AUCH
----------
::

     /std/weapon/combat.c


Last modified: Sat Jun 01 13:28:45 2001 by Tilly

