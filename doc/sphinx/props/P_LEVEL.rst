P_LEVEL
=======

NAME
----
::

    P_LEVEL                       "level"                       

DEFINIERT IN
------------
::

    /sys/living/description.h

BESCHREIBUNG
------------
::

     Spieler-Level (!= Magierlevel)

     In Krankheits- (CL_DISEASE) und Giftobjekten (CL_POISON) drueckt 
     P_LEVEL aus, wie schwer die Krankheit/das Gift ist. Je nachdem, wie 
     hoch oder niedrig der Level gesetzt wird, muss z.B. ein Kleriker mehr 
     oder weniger oft kurieren, um  eine (ggf. stufenweise) Heilung zu 
     bewirken.

     In Fluchobjekten (CL_CURSE) gibt P_LEVEL ebenfalls die Schwere des
     Fluches an, jedoch ist hier zu beachten, dass z.B. Kleriker aktuell
     keine stufenweise Schwaechung bewirken koennen, sondern entweder den
     Fluch entfernen koennen oder komplett scheitern. 
     Der Fluchlevel ist hier grob als Chance des Scheiterns in einem 
     Wertebereich von 1-100 zu sehen, was bedeutet, dass ein Fluchlevel 
     von 100 als permanenter, nicht entfernbarer Fluch anzusehen ist.

     Genaueres ist in der Klerusgilde nachzulesen oder beim GM Klerus zu
     erfragen.

SIEHE AUCH
----------
::

     Properties:  P_GUILD_LEVEL
     Allgemeines: /doc/wiz/gift, /doc/help/gift
     Funktionen:  AddClass(L), is_class_member(L)

Letzte Aenderung: 2015-Feb-02, Arathorn.

