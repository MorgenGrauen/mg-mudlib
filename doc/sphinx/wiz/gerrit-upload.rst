Upload via Gerrit
=================

Zunaechst sei einmal auf die Manpage :doc:`git-workflow` verwiesen.

Upload / Push ohne Review
-------------------------

Ihr folgt dem :doc:`git-workflow` einfach vollstaendig. Eure Aenderungen
werden direkt in die aktive Mudlib uebertragen.

Upload / Push mit Review
------------------------

Folgt dem :doc:`git-workflow` bis Schritt 4 (einschliesslich). Ab dort gehts
wie folgt weiter:

  5. Pusht eure Aenderungen zum Review nach Gerrit:
     ``git push origin lokalerbranch:refs/for/master``
     Damit werden eure Aenderungen des Zweigs lokalerbranch in den Review fuer
     den master-Zweig im MG uebertragen.
     Fuer jeden Commit wird ein Change in Gerrit erzeugt.
     Ihr bekommt in der Ausgabe auch die URL zum Change, den ihr erzeugt habt.
     Anschliessend tragt ihr entweder dort auf der Weboberflaeche einen oder
     mehrere Reviewer ein oder redet mit denjenigen im Mud.

  6. Die Reviewerin hat oft Wuensche, d.h. ihr muesste den Change nochmal
     ueberarbeiten. Dazu gibt es zwei Moeglichkeiten:

     a) Ihr aendert auf der Weboberflaeche selber (``Edit`` und spaeter dann
        ``Publish Edit``
     b) Wollt ihr bei euch lokal auf dem Rechner nur einen (den letzten Commit
        in eurem lokalen Zweig) aendern geht das am
        besten mit ``git commit --amend``.
        Spaetestens nach dieser Aenderungen **muss** in der Commit-Message
        die sog. Change-ID aus Gerrit als *letzte Zeile* und mit einer
        Leerzeile getrennt auftauchen, z.B.
        ``Change-Id: Ie8fe15834a0eb3917318dc291b97a28e34b85b75``.
        Anschliessend koennt ihr wie in Schritt 4. erneut pushen und der
        Change in Gerrit wird aktualisiert.
     c) Wollt ihr mehrere Commits bei euch lokal auf dem Rechner aendern,
        muesst in eurem lokalen Zweig ein sog. interaktives rebase machen und
        in diesem dann eure Commits editieren.
        Wechselt in euren lokalen Zweig und benutzt ``git rebase -i master``
        (dies bedeutet "setze diesen Zweig neu auf den master auf und lass
        mich dabei Aenderungen vornehmen").
        Spaetestens hierbei **muss** in allen Commit-Messages die zu diesem
        Commit passende Change-ID aus Gerrit als *letzte Zeile* und mit einer
        Leerzeile getrennt auftauchen.
        Anschliessend koennt ihr wie in Schritt 4. erneut pushen und alle
        Changes in Gerrit werden aktualisiert.

  7. Wenn die Reviewerin zufrieden ist, wird sie den Change in den Zweig
     'master' mergen und er landet damit im MG.


Weitere Infos
-------------

Es gibt noch viele weitere Informationen, wie man Aenderungen in Gerrit
bekommt (z.B. kann man direkt mit dem ersten Push automatisiert Reviewer fuer
alle Changes festlegen, Topics und Hashtags zur Kategorisierung verwenden). Es
ist empfehlenswert, in die Doku von Gerrit zu schauen:

  * `<https://mg.mud.de/gerrit/Documentation/user-upload.html>` 
  * `<https://mg.mud.de/gerrit/Documentation/user-upload.html#push_create>` 
  * `<https://mg.mud.de/gerrit/Documentation/user-upload.html#push_replace>` 

Mehr Informationen ueber die Change-ID bekommt ihr hier:
`<https://mg.mud.de/gerrit/Documentation/user-changeid.html>` 

Ihr koennt git auch beibringen, dass in alle eure Commit-Messages die sog.
Change-ID automatisch ergaenzt wird, in dem ihr einen 'commit-msg' hook aus
Gerrit in euerem Repository installiert:
``scp -p -P 29418 <user>@mg.mud.de:hooks/commit-msg .git/hooks/``
Details bekommt ihr hier:
`<https://mg.mud.de/gerrit/Documentation/user-changeid.html#creation>` 

Die Doku geht auch auf haeufige Taetigkeiten im Zusammenhang mit dem Upload
von Aenderungen ein:
`<https://mg.mud.de/gerrit/Documentation/user-changeid.html#_git_tasks>` 


Siehe auch
----------

  * gerrit
  * git-howto: Wie git benutzt wird
  * git-workflow: Wie man sinnvoll vorgehen kann beim Entwickeln
  * git-kooperation: Ein ueber git-workflow hinausgehendes Beispiel zur
    Synchronisation bzw Kooperation mehrerer Magier/Rechner
  * gerrit-sync: Wie die Synchronisierung zw. git-Repos und Mudlib ablaeuft
  * git-faq: haeufig gestellte Fragen/Probleme

