P_REAL_RACE
===========

NAME
----

	P_REAL_RACE				"real_race"

DEFINIERT IN
------------

	/sys/living/description.h

BESCHREIBUNG
------------
  
  Diese Property enthaelt die Rasse des Livings. Sie darf nicht durch 
	Shadows ueberschrieben werden.	

	Wirklich interessant ist sie, wenn ein Spieler sich tarnt. Dort kann
	man mit dieser Property trotz Tarnung feststellen, welche Rasse der
	Spieler hat.

	Bei NPC enthaelt sie den gleichen Wert wie P_RACE. Wenn P_REAL_RACE
	allerdings gesetzt wird, kann man damit einen getarnten NPC simu-
	lieren, da dann P_RACE und P_REAL_RACE voneinander abweichen.


BEISPIEL
--------

.. code-block:: pike

  // Ein Zwerg mag Zwergenbrot, fuer Elfen ist es giftig. Selbst wenn der
  // Elf sich als Zwerg tarnt, wird ihm durch lembas sicher uebel werden:
  int futter(string arg) {
    notify_fail("Was willst Du essen?\n");
    if(!arg || !id(arg)) return 0;

    notify_fail("Du kannst nichts mehr essen.\n");
    if(!this_player()->eat_food(55)) return 0;

    write("Du isst ein Stueck Zwegenbrot. Du versuchst es zumindest!\n");
    say(sprintf("%s beisst in ein Stueck Zwergenbrot. Zahnschmerz!!!\n",
        this_player()->Name()));

    switch( this_player()->QueryProp(P_REAL_RACE) )
    {
      case "Zwerg":
        if ((this_player()->QueryProp(P_RACE))!="Zwerg")
          write("Zur Tarnung spuckst Du etwas von dem Brot aus!\n"); 
        this_player()->buffer_hp(100,10);
        this_player()->buffer_sp(100,10);
        break;

      case "Elf":
        write("Das Zwergenbrot brennt wie Feuer auf Deiner Zunge!");
        // Getarnt?
        if ((this_player()->QueryProp(P_RACE))!="Elf")
          write(" Deine Tarnung nutzt Dir da wenig.\n"
        else
          write("\n");
        this_player()->restore_spell_points(-100);
        this_player()->do_damage(100,this_object());
        break;

      default:
        write("Du bekommst nur wenig davon herunter..\n");
        this_player()->buffer_hp(10,1);
        this_player()->buffer_sp(10,2);
        break;
    }
    remove();
    return 1;
  }

	

SIEHE AUCH
----------
  
  :doc:`P_RACE`
	/std/living/description.c, /sys/living/description.h,


Last modified: 15.10.2018 Zesstra
