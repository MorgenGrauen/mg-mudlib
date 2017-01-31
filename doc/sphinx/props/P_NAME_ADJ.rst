P_NAME_ADJ
==========

NAME
----
::

     P_NAME_ADJ "name_adj"

DEFINIERT IN
------------
::

     <thing/description.h>

BESCHREIBUNG
------------
::

     Diese Property enthaelt ein oder mehrere Adjektive in Form eines Arrays
     von Strings. Es ist nur der Wortstamm anzugeben! Die Adjektive werden
     von der Funktion name() dekliniert und vor den Namen gesetzt, wirken
     also als Aufzaehlung von Adjektiven vor dem Namen.

     Die hier angegebenen Adjektive dienen nur zur Ausgabe! Soll sich das
     Objekt auch ueber Adjektive ansprechen lassen, muss man diese mit
     AddAdjective() uebergeben.

     Soll das Objekt nur ein einzelnes Namensadjektiv besitzen, kann man dem
     SetProp()-Aufruf auch einen String uebergeben; gespeichert wird die
     Property aber in jedem Fall als Array.

     Wenn ein Adjektiv unregelmaessig ist, kann man die vier Faelle auch
     als Array uebergeben. Man muss dann aber Arrays schachteln, damit von den
     mehrfachen Adjektiven unterschieden werden kann.

	

BEISPIELE
---------
::

     SetProp(P_NAME, "Hut");
     SetProp(P_NAME_ADJ, "alt");

     name(WESSEN,1)      => "des alten Huts"


     // Zwei Adjektive, gleichrangig zu Substantiv
     SetProp(P_NAME_ADJ, ({"alt", "gammelig"}));

     name(WESSEN,1)      => "des alten, gammeligen Huts"


     // Zwei Adjektive, erstes ist Attribut zu zweitem
     falsch:  SetProp(P_NAME_ADJ, ({"gruen", "gestreift"}));
              name(WESSEN,1)      => "des gruenen, gestreiften Huts"
     richtig: SetProp(P_NAME_ADJ, ({"gruen gestreift"}));
              name(WESSEN,1)      => "des gruen gestreiften Huts"

     // Unregelmaessiges Adjektiv
     SetProp( P_NAME_ADJ,({({"rosa","rosa","rosa","rosa"})});
              name(WESSEN,1)         => "des rosa Huts"
     // Ohne inneres Array haette man 4 mal rosa als Adjektiv
     // => "des rosaen, rosaen, rosaen, rosaen Huts"

SIEHE AUCH
----------
::

     /std/thing/description.c, name(), P_NAME, DeclAdj()

23.April 2007 Rumata

