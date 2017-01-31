ChangeReputation()
==================

FUNKTION
--------
::

     public varargs int ChangeReputation(string repid, int value, int silent)

DEFINIERT IN
------------
::

     /std/player/reputation.c

ARGUMENTE
---------
::

     repid
       Jede neue Reputationsgruppe muss anfangs mit einer eindeutigen ID von 
       einem EM in den Reputationsmaster eingetragen werden. Danach kann man 
       ueber die eindeutige ID <repid> auf sie zugreifen.
     value
       Der Wert, um den die Reputation geaendert werden soll. Positive Werte 
       erhoehen die Reputation, negative verschlechtern sie.
     silent
       Ein optionales Flag. Falls gesetzt, wird keine Standardmeldung ueber 
       die Reputationsaenderung an den Spieler ausgegeben. Man koennte dann 
       eigene Meldungen ausgeben.

BESCHREIBUNG
------------
::

     Vor der Aenderung wird ein Check auf die UID des ausfuehrenden Objektes
     ausgefuehrt, "fremde" Reputationen darf man somit nicht veraendern.
     Man kann aber selbstverstaendlich in begruendeten Faellen mit dem
     zustaendigen Magier/Regionsmagier sprechen, ob man ebenfalls Zugriff
     erhaelt. Eingetragen wird dies schlussendlich durch einen EM.

RUeCKGABEWERT
-------------
::

     REP_RET_SUCCESS    Reputation wurde veraender.
     REP_RET_SUCCESSCUT Reputation wurde auf Min / Max veraendert
     REP_RET_WRONGARGS  Falsche Argumente fuer ChangeRep()
     REP_RET_INVALIDUID Unzulaessige UID / keine Zugriffsrechte
     REP_RET_ALREADYMAX Reputation bereits Max / Min
     REP_RET_INACTIVE   Reputation momentan inaktiv
     REP_RET_INVALIDREP Reputation nicht vorhanden

BEISPIELE
---------
::

     s. reputation

SIEHE AUCH
----------
::

     reputation
     GetReputation(), GetReputations()

ZULETZT GEAeNDERT
-----------------
::

06.04.2009, Zesstra

