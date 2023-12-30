P_WEAPON_TYPE
=============

NAME
----
::

     P_WEAPON_TYPE "weapon_type"

DEFINIERT IN
------------
::

     <weapon.h>

BESCHREIBUNG
------------
::

    Um was fuer eine Waffe handelt es sich? Es stehen verschiedene
    Typen zur Auswahl. Man sollte hier nur die in <combat.h> definierten
    Konstanten verwenden:

       WT_AMMU           Munition fuer Fernkampfwaffen (Hinweis beachten)
       WT_AXE            Axt
       WT_CLUB           Keule
       WT_HANDS          blosse Haende
       WT_KNIFE          Messer, Dolch
       WT_RANGED_WEAPON  Fernkampfwaffe
       WT_SPEAR          Speer
       WT_STAFF          Kampfstab
       WT_SWORD          Schwert
       WT_WHIP           Peitsche
       WT_MISC           Sonstiges

   Der Waffentyp WT_MISC ist schnoedem Tand und nutzlosem Kram vorbehalten.
   Waffen dieses Typs duerfen keine P_WC > 0 besitzen oder kampfrelevante
   Bedeutung haben.


HINWEISE
--------
::

   WT_AMMU ist fuer Munition gedacht, die mit Fernwaffen verschossen werden
   kann. Da es nicht vorgesehen ist, Munition zu zuecken, sollten derartige
   Objekte nicht /std/weapon erben. Da man jedoch oft groessere Mengen
   Munition benoetigt, ist zu empfehlen, Munitionsobjekte stattdessen von
   /std/unit abzuleiten.

   WT_AMMU ist in der Regel auch nicht fuer Artillerie-Objekte noetig, ausser
   diese sollen gleichzeitig Artillerie und Munition sein.

Letzte Aenderung: 12. Dezember 2023, Arathorn.

