P_COMBATCMDS
============

NAME
----
::

    P_COMBATCMDS                  "combatcmds"                  

DEFINIERT IN
------------
::

    /sys/properties.h

BESCHREIBUNG
------------
::

     Fuer den Kampf gebrauchbare Befehle spezieller Objekte (damit auch
     Monster sie automatisch richtig anwenden koennen)
     Der Inhalt von P_COMBATCMDS ist ein Mapping, der Key ist das Kommando,
     um den Gegenstand zu benutzen (also z.B. "wirf flammenkugel"), und der
     Value ein weiteres Mapping mit Zusatzinfos (definiert in /sys/combat.h).
     Folgende Keys sind definiert:
     - C_MIN, C_AVG, C_MAX:
       minimaler, mittlerer und maximaler Schaden, den das
       Objekt macht. Alle Angaben in LEBENSPUNKTEN, d.h. Defend-Einheiten/10.
       Bei einem Aufruf wie 'enemy->Defend(200+random(200), ...)' ist dann
       C_MIN=20, C_AVG=30, C_MAX=40.
     - C_DTYPES:
       Array mit dem Schadenstyp oder den Schadenstypen. Beim Eisstab
       wuerde der Eintrag dann 'C_DTYPES:({DT_COLD})' lauten.
     - C_HEAL:
       Sollte das Kampfobjekt ueber die Moeglichkeit verfuegen, den Anwender
       irgendwie zu heilen, so wird hier die Heilung in LP/MP eingetragen.
       Das funktioniert auch bei Objekten, die nur heilen, also sonst
       nichts mit Kampf zu tun haben.
       Im Lupinental z.B. gibt es Pfirsiche, die beim Essen 5LP heilen. Da
       kann man dann 'SetProp(P_COMBATCMDS, (["iss pfirsich":([C_HEAL:5])]))'
       eintragen.
     Es sind auch mehrere Kommandos moeglich, z.B. bei Objekten, die sowohl
     heilen als auch Kampfwirkung haben.

