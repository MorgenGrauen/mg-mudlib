query_userlist()
================

FUNKTION
--------

  public mixed query_userlist(string user, int prop)

DEFINIERT IN
------------

  /secure/master/userinfo.c

ARGUMENTE
---------

  user
    UID des abzufragenden Users
  prop
    Datum aus der Userliste, welches abgefragt werden soll

BESCHREIBUNG
------------

  Aus der Userliste des MG wird eine der folgenden Informationen ueber den
  Spieler <user> abgefragt:

  USER_LEVEL (int)
    Magierlevel (1 fuer Seher)
  USER_DOMAIN (string*)
    Liste der Regionen, in der der Magier RM ist
  USER_OBJECT (string)
    Shell des Spielers
  USER_CREATION_DATE (int)
    Zeit des ersten Logins
  USER_GUILD (string*)
    Gilden, fuer welche die Spielerin Gildenmagierin ist
  USER_EKTIPS (string)
    Fuer welche EKs hat der Spieler einen Tip erhalten? Rueckgabe als
    Bitstring, in welchem die entsprechenden Bits gesetzt sind.
  USER_FPTIPS (string)
    Fuer welche FPs hat der Spieler einen Tip erhalten? Rueckgabe als
    Bitstring, in welchem die entsprechenden Bits gesetzt sind.
  USER_UIDS_TO_TAKE_CARE (string*)
    UIDs, fuer welche die Magierin zustaendig ist. Hierbei sind nur explizit
    eingetragene in diesem Array, nicht welche, fuer sie implizit kraft ihres
    Amtes zustaendig ist (z.B. als GM oder RM).

  Die o.a. Konstanten sind in <userinfo.h> definiert.

  Wenn der Spieler nicht existiert, wird 0 zurueckgegeben. Ein nicht
  unterstuetztes <prop> loest einen Laufzeitfehler aus.

SIEHE AUCH
----------

  :doc:`find_userinfo`

Last modified: 21.01.2020, Zesstra

