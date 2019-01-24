Erstellung neuer Repositories in Gerrit
-------------------------------------------

Neue Repositories werden wie bisher aus dem Mud erstellt. Dies darf jeder, der
Schreibrechte in dem Verzeichnis m Mud besitzt.
Zur vollen Stunde werden Projekte fuer alle Verzeichnisse erzeugt, welche eine
Datei gerrit-create-project enthalten. Dies ist unterschiedlich, je nachdem,
was in der ersten Zeile dieser Datei steht:

  import
    Das Projekt wird erzeugt und der Inhalt des Verzeichnissesaus dem Mud
    importiert.
  create
    Das Projekt wird LEER erzeugt und nichts importiert. Dies ist dann
    nuetzlich, wenn man eine bestehen Historie aus einem anderen git-Repo
    uebernehmen will. Diese kann man dann in das neue, leer Repo pushen. Erst
    danach erfolgt auch die Synchronisation mit dem Mud. (Tip: beim push
    sollte master den gleichen Inhalt haben wie im Mud liegt.)

Erzmagier haben zusaetzlich die Moeglichkeit, Projekte ohne Zeitverzoegerung
ueber die Weboberflaeche anzulegen.

Nur ueber die Anlage durch einen EM lassen sich Verzeichnisse ausserhalb von
/d/, /p/service und /players/ sowie Verzeichnisse fuer nicht-existente Magier
erzeugen.

Hinweise fuer Erzmagier / Admins
--------------------------------

- Beim Erstellen von Projekten per Weboberflaeche:

  + "Creatw New" in der Repositoryliste
  + Name in der Form  d/region/magier/bla (kein / am Anfang, keiner am Ende
    und auch kein .git am Ende)
  + Rechtetemplat: meist d/region - aber bei secure/-Verzeichnissen in dem
    Verzeichnis *muss* es All-Projects sein, Eigentuemer Erzmagier (damit Du die
    Rechte nach dem Erstellen noch anpassen darfst...)
  + und ganz wichtig: 'create initial commit' auf True, nur dann importiert er
    beim Anlegen aus dem Mud. Im anderen Fall ist das Repo leer.
  + Nach Anlegen sollte in /d/ jeweils die Gruppen RM-Region und u_<magier>
    "Owner"-Rechte auf refs/* erhalten.

- Laesst man ein Projekt (nachtraeglich) die Rechte von All-Projects (dem
  glob. Default) erben
  (z.B. wenn Projekt ein secure/ enthaelt): bitte _vorher_ den Gruppen
  Erzmagier, u_<magier> und ggf. dem passenden RM-* "Owner"-Rechte auf refs/*
  geben.
- Beim Loeschen von Projekten in gerrit muss das Projekt haendisch auch
  in ~/gerrit/ geloescht werden.

