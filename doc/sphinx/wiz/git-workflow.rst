Git workflow
============

Typischer Arbeitsablauf
-----------------------

(Es gibt andere Arbeitsweisen, aber dies hier ist eine, die sich bewaehrt
 hat.)

Nehmen wir an, ich moechte etwas neues einbauen, einen Bug fixen etc.
Alle der folgenden Schritt werden auf eurem eigenen Rechner in eurem lokalen
Clone des jeweiligen Repositories durchgefuehrt.

1. Repository clonen und/oder updaten::

     git clone ssh://mgg/dings/bums
     git checkout master
     git pull

  Zuerst einmal wird ein checkout des Zweiges 'master' gemacht. Und in diesen
  Zweig hol ich mir erstmal den aktuellen Stand aus dem Mud (pull).

  Jetzt moechte ich alle Aenderungen erstmal in einem separaten Zweig machen.
  Warum? Weil dann der 'master' (das ist der aktive Zweig im Mud!) immer in
  einem benutzbaren Zustand ist. Desweiteren kann man einen Zweig einfach
  wegwerfen, wenn irgendwas groesseres schiefgelaufen ist...
  Ausserdem ist es dann einfacher, zwischenzeitliche Aenderungen aus dem Mud zu
  integrieren.

2. Neuen Zweig erstellen::

    git checkout -b neue_kampftaktik

  Hiermit wird  ein neuer Zweig erstellt und gleichzeitig in ihn gewechselt.

  Hier mach ich jetzt alle moeglichen Arbeiten und Basteleien, die ich fuer die
  neue Kampftaktik brauche. Tipps dazu:

    * Viele und haeufige Commits machen! Je kleiner einzelne Commits sind,
      desto besser kann man Aenderungen spaeter verfolgen (was z.B. super ist,
      wenn jemand was abnehmen muss!) und desto besser kann man einzelne
      Aenderungen spaeter bei Bedarf auch rueckgaengig machen, wenn man merkt,
      dass man stellenweise Unsinn gebaut hat. ;-)
    * Thematisch unterschiedliche Dinge in verschiedene Commits packen. Also
      z.B. erst Syntaxfehler editieren und commiten, dann eine neue Methode
      fuer etwas ganz andere schreiben und commiten.

3. Aenderungen pruefen und commiten

  Hiermit lasse ich mir anzeigen, welche Dateien nicht-committete Aenderungen
  enthalten (oder neu sind/geloescht wurden)::

    git status

  Dies zeigt mir alle nicht-committeten Aenderungen an - zeilenweise
  verglichen mit dem vorherigen Zustand::

    git diff


4. Aenderungen in lokales Repository commiten

  Hiermit merke gemachten Aenderungen fuer den naechsten Commit vor.
  Ich kann hier einzelne Dateien auswaehlen, bestimmte Dateien oder sogar nur
  bestimmte Teile der Aenderungen in einer Datei. (Hierzu bitte die
  Git-Doku bemuehen.)::

    git add <file>             // einzelne Files auswaehlen
    git add -A ./              // alle Files auswaehlen

  Hiermit erstelle ich einen Commit, der die bisherigen Aenderungen umfasst.
  Dabei wird ein Editor geoeffnet, in den ich eine informative Nachricht
  ueber meine Aenderungen hinterlassen kann. Das ist besonders wichtig, wenn
  ich in fremden Gebieten arbeite, aber auch fuer mich und einen etwaigen
  abnehmenden Magier sehr sinnvoll.
  Anregung: Die erste Zeile ist das sog. Betreff des Commits - vergleichbar
  mit dem Betreff einer eMail. (bitte nur so 50-60 Zeichen). Anschliessend
  muss eine leere Zeile folgen und danach eine laengere Beschreibung eingeben
  werden::

    git commit

  Wenn ich an diesem Punkt mit dem Bugfix oder Feature noch nicht fertig bin:
  einfach den Schritt 4 beliebig oft wiederholen, d.h. beliebig viele weitere
  Commits machen.

5. Aenderungen in lokalen Master-Zweig mergen

  Bin ich dann schliesslich aber mal fertig, gehe ich erstmal zurueck zum
  master-Zweig::

    git checkout master

  Huch! Ploetzlich sind alle Dateien auf dem alten Stand! Keine Sorge,
  unsere Aenderungen sind im Zweig 'neue_kampftaktik' sicher verwahrt.

  Achtung: wenn ihr mit anderen zusammen arbeitet, koennte jemand
  anderes im MUD Aenderungen vorgenommen haben. Ein einfaches ``git pull``
  um die Dateien im 'master'-Zweig auf den neuesten Stand zu bringen,
  zeigt euch auch Aenderungen. Wenn da jetzt
  ``Already up-to-date.`` steht, ist alles in Butter, ansonsten siehe unten.

  Mit diesem Kommando hole ich nun alle Aenderungen aus meinem Zweig
  'neue_kampftaktik' in den Zweig 'master' (merge)::

    git merge neue_kampftaktik

6. Aenderungen in das MUD-Repository uebertragen
   Jetzt bin ich bereit, die Aenderungen ins Mud zu uebertragen::

    git push origin <lokaler_zweigname>:<zweigname_im_mg>

  Job done!
  (Hinweis: nur der Branch "master" wird ins Mud selber synchronisiert, alle
  anderen Zweige existieren nur in den git-Repositories.)

Sonderfaelle und erweiterte Moeglichkeiten
------------------------------------------

A) Zwischenzeitliche Aenderungen im MUD beruecksichtigen

  Es koennte sein, dass man fuer den Branch ne ganze Weile gebraucht hat -
  und dass waehrend meiner Arbeit jemand anders Aenderungen (im Mud oder
  Repo) gemacht hat.

  Diese Aenderungen sollte man sich wie geschrieben als erstes nach dem
  Umschalten zum master-Zweig holen::

    git pull

  Jetzt geh ich wieder in meinen Zweig (ohne -b)::

    git checkout neue_kampftaktik

  und mache ein sog. Rebase. Damit verschiebe ich sozusagen, den Punkt, an dem
  mein Zweig vom 'master' abzweigt und tue so, als ob die eben geholten
  Aenderungen schon da gewesen waeren, als ich den Zweig erstellte.
  (Andere Sichtweise: ich nehme meine Zweig und setz ihn auf den aktuellen
  'master' dran.)::

    git rebase master

  Der Vorteil ist: wenn jetzt was kaputt gegangen ist, es also Konflikte gibt,
  dann gibt es die nur in meinem Zweig 'neue_kampftaktik' und dort kann ich
  sie in aller Ruhe reparieren. Sowohl der 'master' im MUD als auch mein
  lokaler 'master' sind intakt.

  Und jetzt geht es wie oben weiter.


SIEHE AUCH
----------

  * gerrit
  * Doku von Gerrit:

    * `<https://mg.mud.de/gerrit/Documentation/intro-user.html>` 
    * `<https://mg.mud.de/gerrit/Documentation/index.html#_tutorials>` 

  * `gerrit-upload`: Wie man Dinge in Gerrit hochlaedt
  * git-howto: Wie git benutzt wird
  * git-kooperation: Ein ueber git-workflow hinausgehendes Beispiel zur
    Synchronisation bzw Kooperation mehrerer Magier/Rechner
  * gerrit-sync: Wie die Synchronisierung zw. git-Repos und Mudlib ablaeuft
  * git-faq: haeufig gestellte Fragen/Probleme

