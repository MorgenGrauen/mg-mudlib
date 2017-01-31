Pacify()
========

FUNKTION
--------
::

	public int Pacify(object caster)

DEFINIERT IN
------------
::

	/std/living/combat.c

BESCHREIBUNG
------------
::

	Diese Funktion versucht, ein Lebewesen zu befrieden.
  Will eine Gilde ein Lebewesen befrieden, muss sie hierfuer diese Funktion
  in dem Lebewesen aufrufen.

  Ein immer befriedbarer NPC kann durch das Setzen von P_ACCEPT_PEACE in einem
  Lebewesen realisiert werden.

	Standardmaessig funktioniert die Funktion wie folgt:
  * Kommt der Versuch vom Spellcaster selbst, ist er immer erfolgreich.
  * Kommt der Versuch von einem Teamkollegen, ist er immer erfolgreich.
  * Hat das Lebewesen keine Gegner, ist der Versuch immer erfolglos.
  In diesen Faellen erfolgt auch keine Erhoehung des Befriedezaehlers.

  

  In anderen Faellen wird die in P_PEACE_HISTORY fuer die Gilde des Casters
  abgelegte Zahl erfolgreicher Befriedungen (ANZ), die Intelligenz des
  Casters (INT_CASTER) und die Intelligenz des Lebenwesens selber (INT_ME)
  ermittelt.
  Anschliessend wird eine Wahrscheinlichkeit w ausgerechnet:

  

    w = (INT_CASTER + 10 - ANZ*4) / (INT_ME + 10)

  

  Hierbei gibt w die Chance auf eine erfolgreiche Befriedung an. Mittels einer
  Zufallszahl wird bestimmt, ob der aktuelle Versuch erfolgreich ist. Falls
  ja, wird der Zaehler fuer die Gilde des Casters in P_PEACE_HISTORY erhoeht.

  Je oefter ein Lebewesen als von einer Gilde schon befriedet wurde, desto
  unwahrscheinlicher, dass es erneut darauf 'hereinfaellt'.

BEMERKUNGEN
-----------
::

  *	Die Funktion kann auch ueberschrieben werden, um ein vom Magier
    gewuenschtes Verhalten zu realisieren. Ein komplettes Abhaengen von
    Befriedungen sollte dabei aber die Ausnahme sein!
  * Diese Funktion verwaltet auch das P_PEACE_HISTORY, speziell die Reduktion
    der Erfolgszaehler. Ueberschreibt man sie ohne das geerbte Pacify()
    zu nutzen, wird P_PEACE_HISTORY nicht mehr verwaltet.

RUECKGABEWERTE
--------------
::

    1 - das Lebewesen wurde erfolgreich befriedet..
    0 - der Befriedeversuch ist gescheitert.

    

BEISPIELE
---------
::

    Angenommen, der Caster hat eine Intelligenz von 22. Die folgende Tabelle
    gibt dann die Wahrscheinlichkeiten fuer eine erfolgreiche Befriedung an:
    (in Abhaengigkeit von eigener Intelligenz und vergangener erfolgreicher
     Versuche)
 INT_ME  Erfolgswahrscheinlichkeiten je nach Anzahl erfolgreicher Versuche
              1       2       3       4       5       6       7       8
      0     280     240     200     160     120      80      40       0
      2  233,33     200  166,67  133,33     100   66,67   33,33       0
      4     200  171,43  142,86  114,29   85,71   57,14   28,57       0
      6     175     150     125     100      75      50      25       0
      8  155,56  133,33  111,11   88,89   66,67   44,44   22,22       0
     10     140     120     100      80      60      40      20       0
     12  127,27  109,09   90,91   72,73   54,55   36,36   18,18       0
     14  116,67     100   83,33   66,67      50   33,33   16,67       0
     16  107,69   92,31   76,92   61,54   46,15   30,77   15,38       0
     18     100   85,71   71,43   57,14   42,86   28,57   14,29       0
     20   93,33      80   66,67   53,33      40   26,67   13,33       0
     22    87,5      75    62,5      50    37,5      25    12,5       0
     24   82,35   70,59   58,82   47,06   35,29   23,53   11,76       0
     26   77,78   66,67   55,56   44,44   33,33   22,22   11,11       0
     28   73,68   63,16   52,63   42,11   31,58   21,05   10,53       0
     30      70      60      50      40      30      20      10       0
     32   66,67   57,14   47,62    38,1   28,57   19,05    9,52       0
     34   63,64   54,55   45,45   36,36   27,27   18,18    9,09       0
     35   62,22   53,33   44,44   35,56   26,67   17,78    8,89       0
     36   60,87   52,17   43,48   34,78   26,09   17,39     8,7       0
     38   58,33      50   41,67   33,33      25   16,67    8,33       0
     40      56      48      40      32      24      16       8       0
     42   53,85   46,15   38,46   30,77   23,08   15,38    7,69       0
     44   51,85   44,44   37,04   29,63   22,22   14,81    7,41       0
     46      50   42,86   35,71   28,57   21,43   14,29    7,14       0
     48   48,28   41,38   34,48   27,59   20,69   13,79     6,9       0
     50   46,67      40   33,33   26,67      20   13,33    6,67       0
     52   45,16   38,71   32,26   25,81   19,35    12,9    6,45       0
     54   43,75    37,5   31,25      25   18,75    12,5    6,25       0
     56   42,42   36,36    30,3   24,24   18,18   12,12    6,06       0
     58   41,18   35,29   29,41   23,53   17,65   11,76    5,88       0
     60      40   34,29   28,57   22,86   17,14   11,43    5,71       0
     62   38,89   33,33   27,78   22,22   16,67   11,11    5,56       0
     64   37,84   32,43   27,03   21,62   16,22   10,81    5,41       0
     66   36,84   31,58   26,32   21,05   15,79   10,53    5,26       0
     68    35,9   30,77   25,64   20,51   15,38   10,26    5,13       0
     70      35      30      25      20      15      10       5       0
     72   34,15   29,27   24,39   19,51   14,63    9,76    4,88       0
     74   33,33   28,57   23,81   19,05   14,29    9,52    4,76       0
     76   32,56   27,91   23,26    18,6   13,95     9,3    4,65       0
     78   31,82   27,27   22,73   18,18   13,64    9,09    4,55       0
     80   31,11   26,67   22,22   17,78   13,33    8,89    4,44       0
     82   30,43   26,09   21,74   17,39   13,04     8,7    4,35       0
     84   29,79   25,53   21,28   17,02   12,77    8,51    4,26       0
     86   29,17      25   20,83   16,67    12,5    8,33    4,17       0
     88   28,57   24,49   20,41   16,33   12,24    8,16    4,08       0
     90      28      24      20      16      12       8       4       0
     92   27,45   23,53   19,61   15,69   11,76    7,84    3,92       0
     94   26,92   23,08   19,23   15,38   11,54    7,69    3,85       0
     96   26,42   22,64   18,87   15,09   11,32    7,55    3,77       0
     98   25,93   22,22   18,52   14,81   11,11    7,41     3,7       0
    100   25,45   21,82   18,18   14,55   10,91    7,27    3,64       0

SIEHE AUCH
----------
::

        P_ACCEPT_PEACE, P_PEACE_HISTORY

LETZTE AENDERUNG
----------------
::

07.06.2008, Zesstra

