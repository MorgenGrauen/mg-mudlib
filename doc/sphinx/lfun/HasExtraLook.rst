HasExtraLook()
==============

FUNKTION
--------

     public int HasExtraLook(string key);


DEFINIERT IN
------------
    
    /std/living/description.c


ARGUMENTE
---------
    
    string key: Schluesselwort, unter dem der Eintrag registriert wurde. 


BESCHREIBUNG
------------
    Die Funktion kann verwendet werden, um herauszufinden, ob ein
    bestimmter Extralook bei einem Lebewesen eingetragen wurde.

    Es ist hierbei sichergestellt, dass nur solche Extralooks ausgegeben
    werden, die noch nicht abgelaufen sind.


RUECKGABEWERTE
--------------

    - 1, falls der Key enthalten ist
    - 0 sonst.


SIEHE AUCH
----------

   Verwandt:
     :doc:`RemoveExtraLook`, :doc:`AddExtraLook`, :doc:`../props/P_INTERNAL_EXTRA_LOOK`
   Fuer Spielerobjekte:
     :doc:`../props/P_EXTRA_LOOK`

28. August 2019 Arathorn
