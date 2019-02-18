clean_up()
==========

FUNKTION
--------

   int clean_up(int ref);

DEFINIERT IN
------------

   * /std/room.c
   * man kann die Funktion jedoch auch in beliebigen Objekten selbst
     definieren.

ARGUMENTE
---------

   ref
       Ein Referenzzaehler (s.u.)

BESCHREIBUNG
------------

   Wenn ein Objekt seit langer Zeit nicht mehr benutzt wurde (d.h. es wurde
   keine lfun *von aussen* in ihm gerufen), ruft der Driver diese Funktion
   auf. Es kann es sich hier selbst zerstoeren, um Speicher freizugeben (und
   ggf. spaeter mit aktuellem Sourcecode neu erzeugt werden, was auch eine
   Art einfacher Updatemechanismus ist).

   Der Referenzzaehler <ref> gibt an, wie oft das Objekt als Vorlage fuer
   Clones oder erbende Programme dient. Folgende Faelle sind damit
   unterscheidbar:

     ref==0:
       Das Objekt ist ein Clone (oder eine Blueprint mit ersetztem Programm)
     ref==1:
       Das Objekt ist eine Blueprint (kein Clone), von welcher zur Zeit keine
       Clones oder erbenden Programme existieren.
     ref > 1:
       Das Objekt dient als Vorlage / Blueprint fuer Clones oder erbende
       Programme.

   Das Objekt im clean_up zu zerstoeren, ist in der Regel ist daher nur
   sinnvoll, wenn ref kleiner oder gleich 1 ist.

   Sollte man einem Objekt ein clean_up geben, was als Clone im Umlauf ist und
   dieses im Falle von ref==0 zerstoeren, bedeutet dies letztendlich, dass
   sich ungenutzte Clones aufraeumen.

   In einem Spezialfall kann ref auch < 0 sein: wenn clean_up *nicht* vom
   Driver gerufen wird, sondern (manuell) von irgendwo aus der Mudlib, ist
   die *Konvention*, einen Wert < 0 zu uebergeben.

   Standardmaessig definieren nur Raeume clean_up(). Hierbei gilt die
   Besonderheit, dass Raeume sich erst beim *zweiten* Aufruf von clean_up()
   wegraeumen, wenn in ihnen Objekte liegen, die der Raum *nicht* selbst
   mittels AddItem() erzeugt hat. D.h. laesst jemand etwas im Raum rumliegen,
   erhoeht sich effektiv die Cleanup-Zeit auf das doppelte. Und hat
   irgendjemand sich auf einen Hook des Raums registriert, raeumt der Raum
   sich nicht im clean_up() auf.

   Soll ein Raum sich niemals per clean_up() selber aufraeumen, kann die
   Property :doc:`../props/P_NEVER_CLEAN` gesetzt werden. Aber bitte seid
   hiermit *sehr sparsam* und benutzt das nur in Raeumen, die es wirklich
   benoetigen. Fast alle Raeume kommen super klar, wenn sie bei Bedarf neu
   erzeugt werden.

RUeCKGABEWERT
-------------

   Der Rueckgabewert hat nur dann eine Bedeutung, wenn sich das Objekt
   nicht selbst zerstoert hat.

   Wird 0 zurueckgegeben, so wird clean_up() *nicht erneut* gerufen.
   Ein Rueckgabewert ungleich 0 zeigt hingegen an, dass das Objekt sich
   moeglicherweise beim naechsten Aufruf von clean_up() zerstoeren koennte,
   d.h. der Driver wird es nach Ablauf der Cleanup-Zeit erneut versuchen.

BEMERKUNGEN
-----------

   Die Cleanup-Zeit, die ein Objekt nicht benutzt werden darf, bevor
   clean_up() aufgerufen wird, betraegt momentan 18h (64800 Sekunden).

SIEHE AUCH
----------

   lfuns:
     :doc:`reset`
   Properties:
     :doc:`../props/P_NEVER_CLEAN`
   Konzepte:
     memory

18.02.2019, Zesstra
