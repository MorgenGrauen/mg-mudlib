find_obs()
==========

FUNKTION
--------
::

    object* find_obs(string str, int meth)

DEFINIERT IN
------------
::

    /std/living/put_and_get.c

ARGUMENTE
---------
::

    str     Der String der geparsed werden soll.
    meth    Mit Hilfe dieses Parameters koennen bestimmte Bereichs-
            eingrenzungen vorgenommen werden (definiert in moving.h):

            PUT_GET_NONE - keinerlei Bereichseingrenzung.
            PUT_GET_TAKE - es handelt sich um ein Nehmen von Gegenstaenden
                           also wird das inventory ausgenommen.
            PUT_GET_DROP - es handelt sich um das Entfernen von Gegenstaenden
                           also wird das environment ausgenommen.           

                                                                            

BESCHREIBUNG                                                               
---------------------------------------------------------------------------
::

                                                                            

    Der String (str) muss folgendes Format haben damit Objekte gefunden
    werden.

    <gegenstand> [aus container] [in mir|im raum]

    <gegenstand> kann hierbei sowohl eine Objekt-ID als auch ein
    Gruppenbezeichner wie z.b. "alles" sein.

RUeCKGABEWERT
-------------
::

    Ein Array mit allen Objekten die sich angesprochen fuehlen, oder aber 0.

SIEHE AUCH
----------
::

    drop_obj(), give_obj(), pick_obj(), put_obj(), /std/living/put_and_get.c

