P_NO_REGENERATION
=================

NAME
----
::

     P_NO_REGENERATION    "no_regeneration"

DEFINIERT IN
------------
::

     <living/life.h> und <health.h>

BESCHREIBUNG
------------
::

     Durch das Setzen dieser Property kann man verhindern, das ein Lebewesen
     sich regeneriert.
     Es gibt sieben moegliche Werte, die man durch verodern kombinieren
     kann:
     NO_REG_HP        : es werden keine HP regeneriert
     NO_REG_BUFFER_HP : es werden beim "tanken" keine HP regeneriert
     NO_REG_SP        : es werden keine SP regeneriert
     NO_REG_BUFFER_SP : es werden beim "tanken" keine SP regeneriert
     NO_REG_ALCOHOL   : der Alkoholspiegel wird nicht gesenkt
     NO_REG_DRINK     : der Fluessigkeitsspiegel wird nicht gesenkt
     NO_REG_FOOD      : der Nahrungsspiegel wird nicht gesenkt
     sowie die Konstante NO_REG, die eine Kombination aller moeglichen
     Werte darstellt (quasi das groesstmoegliche Uebel ;).

BEISPIELE
---------
::

     Dieses Lebewesen heilt nur beim "tanken" in der Kneipe, ansonsten
     nicht:

     

     SetProp( P_NO_REGENERATION, NO_REG_HP|NO_REG_SP );

SIEHE AUCH
----------
::

     /std/living/life.c


Last modified: 14-05-2001 by Mupfel

