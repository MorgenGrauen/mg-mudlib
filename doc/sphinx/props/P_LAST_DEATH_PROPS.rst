P_LAST_DEATH_PROPS
==================

NAME
----
::

    P_LAST_DEATH_PROPS                "last_death_props"

DEFINIERT IN
------------
::

    /sys/living/life.h

BESCHREIBUNG
------------
::

     Diese Property enthaelt nach dem Tod eines Spielers ein Mapping mit 
     den Werte aller Properties, die im die() zurueckgesetzt werden.

     Auf diese Weise kann ein Objekt auch im NotifyPlayerDeath() noch 
     auf die Werte zurueckgreifen, obwohl sie bereits geloescht sind.

     Folgende Properties werden so gesichert:

   

     P_POISON, P_FROG, P_ALCOHOL, P_DRINK, P_FOOD , P_BLIND, P_DEAF, 
     P_MAX_HANDS, P_PARA, P_NO_REGENERATION , P_HP, P_SP, P_LAST_DEATH_TIME

