heal_self()
===========

FUNKTION
--------
::

    void heal_self(int points);

ARGUMENTE
---------
::

    points: Die dem Lebewesen zukommende Heilung.

BESCHREIBUNG
------------
::

    Dem Lebewesen werden points Lebens- und Konzentrationspunkte 
    gutgeschrieben und auf die aktuellen addiert. Es werden aber nicht
    die maximalen Werte ueberschritten.

RUECKGABEWERT
-------------
::

    Keiner

BEISPIELE
---------
::

    AddCmd("pflueck&beere","pfluecke_cmd","Was moechtest Du pfluecken?");

    int pfluecke_cmd(string str){
        write("Du pflueckst eine Beere, isst sie und fuehlst Dich gleich "
             +"viel besser.\n");
        this_player()->heal_self(30);
        return 1;
    }

    Der Spieler bekommt hier pro Beere die er pflueckt und isst je 30 LP/KP
    zu seinen momentanen.

BEMERKUNGEN
-----------
::

    heal_self() wird gerne fuer Heilstellen in Gebieten genommen, in denen 
    ein Spieler diese Heilung auch wirklich braucht. Dennoch ist der Einsatz
    unbedingt mit der Heilungsbalance abzusprechen und darauf zu achten, dass
    pro reset() nur eine bestimmte Anzahl an Heilungen ausgegeben werden.

    Bei Heilstellen sollte eine evtl. Heilung des Spielers mit der eigens
    dafuer eingerichteten Funktion check_and_update_timed_key realisiert
    werden.

    Diese Funktion kann nicht verwendet werden, um dem Lebewesen durch
    Uebergabe eines negativen Wertes fuer points Schaden zuzufuegen.

SIEHE AUCH
----------
::

    Verwandt:   restore_spell_points, restore_hit_points, buffer_hp
                buffer_sp, check_and_update_timed_key
    Gegenparts: do_damage, reduce_hit_points, reduce_spell_points
    Props:      P_HP, P_SP
    Konzept:    heilung


Last modified: 10.01.2020 by Arathorn

