add_action(E)
=============

BEMERKUNGEN
-----------

  1. Das Verhalten dieser efun wird durch AddCmd aus /std/thing/commands.c
     komfortabler widergespiegelt. Darauf sollte man zurueckgreifen.
  2. add_action() sollte immer innerhalb von init() benutzt werden
  3. das definierende Objekt muss im inventory des Spielers oder
     environment() des kommandogebenden Lebewesens sein
  4. im init() spaeter eingetragene Kommandos oder spaeter hinzukommende
     Objekte werden bei der Kommandoauswertung VOR den alten beruecksichtigt.
     (Daher bewegt sich das Xtool der Magier regelmaessing neu in das
     Inventory, um immer "erstes" Objekt zu sein.)

BEISPIELE
---------

  1. ein Kommando in einem Schirm

    .. code-block:: pike

      void init() {
        ::init();
        add_action("action_oeffne", "oeffne");
      }

      int action_oeffne(string str) {
        if(stringp(str) && id(str)) // Argument da und bin ich gemeint?
        {
          write("Du oeffnest den Schirm.\n");
          say(break_string(this_player()->Name(WER)+" oeffnet einen Schirm.",
                78));
          return 1;
        }
        notify_fail("Was willst Du oeffnen?\n");
        return 0;
      }

   2. Kommandoblockierer:
      frueher beliebt um Spieler lahmzulegen, da es _alle_ Kommandos 
      triggert. Heute jedoch dafuer eher P_DISABLE_COMMANDS verwenden.
      *Achtung*: siehe Implikation von (4) oben.

     .. code-block:: pike

       add_action("action_nothing", "",1 );
       ...
       int action_nothing(string str) {
         write("Dir sind die Haende gebunden.\n");
         return 1;
       }

