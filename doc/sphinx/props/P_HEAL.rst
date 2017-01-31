P_HEAL
======

NAME
----
::

     P_HEAL                        "heal"                        

DEFINIERT IN
------------
::

     /sys/properties.h

BESCHREIBUNG
------------
::

     Numerischer Wert, der beim Verzehr einer Leiche zu den Lebenspunkten 
     desjenigen hinzugezaehlt wird, der die Leiche isst. Der Wert kann auch 
     negativ sein. Falls die Leiche den maximalen Verfallszustand erreicht 
     hat, wird dieser Wert automatisch auf -4 gesetzt, sofern nicht schon
     ein geringerer Wert eingetragen ist.

     Werte unter -4 sind sehr sparsam und nur in begruendeten Einzelfaellen
     zu setzen. Die Regionsmagier werden auf sinnvolle Wertebereiche in
     ihrem Zustaendigkeitsbereich achten und ggf. Grenzwerte fuer ihre 
     Region festlegen.

     Die Gilden koennen P_HEAL abfragen und eigene, grobe Informationstexte
     ausgeben, die den Spieler ueber den zu erwartenden Effekt beim Essen
     einer Leiche informieren.

     Positive Werte von P_HEAL sind bei der Heilungsbalance als Heilstelle
     zu beantragen.

     Fuer nicht allzu harte NPCs sollte P_HEAL auf 0 gesetzt sein. Dieser
     Wert ist gleichzeitig der Standardwert.

SIEHE AUCH
----------
::

     /std/corpse, QueryHealInfo()

     

31.03.2008 Arathorn

