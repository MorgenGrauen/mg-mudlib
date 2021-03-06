wizlist()
=========

SYNOPSIS
--------
::

  varargs void wizlist(string name, int sortkey ) 	

DESCRIPTION
-----------
::

  wizlist() liefert eine Liste mit verschiedenen Verbrauchsdaten 
	zu Magiern. Es werden dabei normalerweise 21 Eintraege ausgegeben:
  10 vor <name>, <name> selbst und 10 nach <name>.

  Die Liste ist dabei folgendermassen aufgebaut:

	1. WL_NAME
	   Gesammelt werden die Daten pro UID. Hierbei zaehlt aber jede EUID, nicht
     nur die von Magiern.
     Das heisst, Objekte unter /d/polar/vanion/eispalast
	   werden nicht unter "vanion" sondern unter "d.polar.vanion"
	   abgerechnet.

	2. WL_COMMANDS
     Anzahl der diesem Objekt zugeordneten commands.

	3. WL_COMMANDS * 100 / total_cmd 
     Prozentualer Anteil an den gesamt verbrauchten commands.

	4. WL_COST (links in der eckigen Klammer)
     Anzahl der verbrauchten eval ticks. Dies ist zeitlich gewichtet, d.h.
     nimmt im Lauf der Zeit ab, wenn nichts mehr dazu kommt.

  5. WL_TOTAL_GIGACOST (rechts in der eckigen Klammer)
     Anzahl der insgesamt verbrauchten eval ticks in Giga-Ticks.
     Nicht zeitlich gewichtet.

	6. WL_HEART_BEATS
     Anzahl der ausgeloesten heart beats.

  

  7. WL_ARRAY_TOTAL

  8. WL_MAPPING_TOTAL

PARAMETERS: 
  Wird name angegeben, wird erzwungen, dass die erwaehnte EUID mit
  in der Liste dargestellt wird. Wird name nicht angegeben, wird es
  automatisch auf this_player()->query_real_name() gesetzt.

  Wird als name "TOP100" angegeben, wird die Top-100-Liste ausgegeben.

  Wird als name "ALL" angegeben, wird die vollstaendige Liste aus-
  gegeben.

  Durch Angabe von sortkey kann die Liste nach einer der Spalten 
  sortiert werden. Gueltige Werte sind die in /sys/wizlist.h ange-
  gebenen Defines.

	

EXAMPLE 
  > xeval wizlist("vanion", WL_HEART_BEATS)
      Erzwingt Vanions Eintrag in der Liste und sortiert die Liste anhand
      der durch die EUIDs ausgefuehrten heart beats.

  > xeval wizlist("ALL", WL_EVAL_COST)
      Zeigt die komplette Liste nach eval ticks-Verbauch sortiert an.
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
::

SEE ALSO
--------
::

      wizlist_info(E)

LAST UPDATED
------------
::

  09.05.2015, Zesstra

