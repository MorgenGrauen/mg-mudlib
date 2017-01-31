P_NAME
======

NAME
----
::

     P_NAME "name"

DEFINIERT IN
------------
::

     <thing/description.h>

BESCHREIBUNG
------------
::

     In dieser Property wird der Name eines Objektes vermerkt. Wenn der Name
     regelmaessig dekliniert wird, reicht ein einfacher String. Wird der
     Name unregelmaessig dekliniert, so kann man ein Array von vier Strings
     mit dem Namen im Nominativ, Genitiv, Dativ und Akkusativ (in dieser
     Reihenfolge) angeben.

     Die Funktion name() behandelt recht viele Sonderfaelle; man sollte den
     Namen also erst einmal in der Form eines einzelnen Strings pruefen.

     Eine Sonderrolle nehmen Unit-Objekte ein: Hier kann man einen Namen
     fuer den Singular und einen Namen fuer den Plural vergeben.

     Setzt man P_NAME eines Unit-Objekts auf einen einfachen String, so wird
     dieser als Name sowohl im Singular als auch im Plural verwendet.

     Uebergibt man ein Array von Strings, so wird der erste Eintrag fuer den
     Singular und der zweite Eintrag fuer den Plural benutzt.

     Bei Unit-Objekten ist es nicht moeglich, auch noch zwischen den
     verschiedenen Faellen zu unterscheiden.

BEMERKUNGEN
-----------
::

     Diese Property sollte nur den reinen Namen enthalten; will man dem
     Namen noch Adjektive voranstellen, so sollte man dies mit P_NAME_ADJ
     bewerkstelligen, also statt

     SetProp(P_NAME, ({ "alter Hut", "alten Huts",
                        "alten Hut", "alten Hut" }) );

     besser

     SetProp(P_NAME, "Hut");
     SetProp(P_NAME_ADJ, "alt");

     Bei Lebewesen wird lower_case(P_NAME) (bei Arrays das erste Element 
     daraus) automatisch als LivingName gesetzt.

BEISPIELE
---------
::

     Ein regelmaessig deklinierbarer Name:

     SetProp(P_NAME, "Arkshat");

     Hier ein Beispiel fuer einen unregelmaessig deklinierbaren Namen:

     SetProp(P_NAME, ({ "Drache", "Drachen", "Drachen", "Drachen" }));

     Und schliesslich der Name eines allseits bekannten Unit-Objektes:

     SetProp(P_NAME, ({ "Muenze", "Muenzen" }));

SIEHE AUCH
----------
::

     /std/thing/description.c, name(), P_NAME_ADJ, set_living_name(),
     find_living(), find_livings()


Last modified: 19. Okt. 2015, Arathorn. 

