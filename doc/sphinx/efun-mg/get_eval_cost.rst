get_eval_cost
=============

BEMERKUNGEN
-----------

  Der Maximalwert betraegt zur Zeit 1.500.000 Ticks (Stand: 2020).

  Sollten die Ticks waehrend der Ausfuehrung irgendwo auf 0 fallen,,
  wird ein Fehler der Art "too long eval" erzeugt.

  Diese Funktion dient dazu, solche Fehler zu verhindern oder genauer zu
  lokalisieren an welchen Stellen im Code wieviel Ticks verbraucht werden.


BEISPIELE
---------

  .. code-block:: pike

      // Ticks zaehlen
      void foo()
      {
        int prev, used, i;

        prev=get_eval_cost(); // Merken, was bis hierhin verbraucht wurde 
        for (i=0;i<=1000;i++) // Dann kommt der zu testende Code, zB eine
        {                     // Schleife
          ...
        }
        used=prev-get_eval_cost(); // Berechnung der Differenz
        printf("Die Schleife verbrauchte %d Ticks.\n", used);
      }

      // Ticks im Auge behalten bei der Ausfuehrung
      foreach(...)
      {
        ... // komplexer code
        if (get_eval_cost() < 100000)
          break;
      }

