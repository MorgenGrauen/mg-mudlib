limited
=======

BEMERKUNGEN
-----------

  Diese Funktion kann bei uns mudlibweit genutzt werden. Allerdings wird
  nur die _Reduktion_ von LIMIT_EVAL zugelassen, alle anderen Limits
  duerfen _nicht_ veraendert werden. Hierbei ist zu beachten, dass das
  Limit fuer LIMIT_EVAL um min. 1000 Ticks unter den noch verfuegbaren
  Ticks liegen muss (get_eval_cost()).
  Fuer LIMIT_COST sind nur 0 und -100 zugelassen.

