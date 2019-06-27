P_NOGET
=======

NAME
----

  P_NOGET                           "noget"


DEFINIERT IN
------------

  /sys/thing/moving.h


BESCHREIBUNG
------------

  Ist diese Property in einem Objekt auf 1 oder einen String gesetzt,
  so kann ein Lebewesen das Objekt nicht nehmen.

  Ist die Property auf 1 gesetzt, wird eine Standardmeldung ausgegeben, 
  beim Nehmen:

    Du kannst <Objektname> nicht nehmen.

  oder wenn man es in einen Behaelter zu stecken versucht:
  
    Du kannst <Objektname> so nirgendwo reinstecken.

  Man kann auch eine eigene Meldung angeben, wobei vom Programmierer
  selbst auf einen korrekten Zeilenumbruch zu achten ist.


BEISPIELE
---------

  Ein Objekt, welches fest im Raum verankert ist, kann natuerlich nicht
  entfernt werden, z.B. ein angebundenes Seil: ::
  
    SetProp(P_NOGET,"Das Seil ist fest am Baum verknotet!\n");

  In einem Kommando zum Losknoten koennte man die Property dann loeschen,
  um ein Wegnehmen zu ermoeglichen.


SIEHE AUCH
----------

    Aehnliches: 
       :doc:`../props/P_NODROP`, :doc:`../props/P_TOO_HEAVY_MSG`, 
       :doc:`../props/P_ENV_TOO_HEAVY_MSG`, :doc:`../props/P_TOO_MANY_MSG`, 
       :doc:`../props/P_NOINSERT_MSG`, :doc:`../props/P_NOLEAVE_MSG`

    Erfolg:
       :doc:`../props/P_PICK_MSG`, :doc:`../props/P_DROP_MSG`, 
       :doc:`../props/P_GIVE_MSG`, :doc:`../props/P_PUT_MSG`,
       :doc:`../props/P_WEAR_MSG`, :doc:`../props/P_WIELD_MSG`
    
    Sonstiges:
       replace_personal(E), /std/living/put_and_get.c

Last modified: 2019-May-06

