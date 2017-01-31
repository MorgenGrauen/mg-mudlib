P_SENSITIVE
===========

NAME
----
::

     P_SENSITIVE                   "sensitive"

DEFINIERT IN
------------
::

     /sys/thing/moving.h

BESCHREIBUNG
------------
::

     Diese Property kann in Objekten gesetzt werden, die auf bestimmte
     Schadensarten empfindlich reagieren sollen. Moegliche Anwendungsfaelle:
     - Das Lebewesen, in dessen Inventar sich ein Objekt befindet, erleidet
       einen Angriff mit der fraglichen Schadensart (Beispiel: eine 
       Pusteblume, die bei Angriff mit Luftschaden auseinanderfaellt).
     - Zwei Objekte treffen im gleichen Environment aufeinander, wobei
       eines empfindlich auf eine Schadensart reagiert, und das zweite diese
       Schadensart mitbringt, d.h. die Empfindlichkeit ausloesen kann.
       (Beispiel: ein feuerempfindlicher Postsack wird angesengt, wenn eine
        brennende Fackel ins gleiche Inventar kommt.)
       Das Ausloesen dieser Empfindlichkeit ist unabhaengig davon, welches 
       Objekt zuerst da war.

     Die Property enthaelt ein Array aus Arrays:
       ({<sensprops_1>, <sensprops_2>, ..., <sensprops_n>})

     

     wobei jeder Eintrag <sensprops> jeweils folgende Struktur hat:
       ({string list_key, string damtype, int treshold, mixed options })

     

     Die Eintraege haben dabei folgende Bedeutung:

     

     list_key: Kann einen von folgenden drei Werten annehmen 
          1) SENSITIVE_INVENTORY, passive Eigenschaft; zeigt an, dass das
             Objekt empfindlich auf aktive Objekte reagiert, die in das
             Inventory des Containers hineinbewegt werden
          2) SENSITIVE_ATTACK, passive Eigenschaft; zeigt an, dass das 
             Objekt empfindlich auf aeussere Einfluesse bzw. Angriffe 
             auf ein Lebewesen reagiert, in dessen Inventar es sich befindet
          3) SENSITIVE_INVENTORY_TRIGGER, aktive Eigenschaft; zeigt an, dass
             das Objekt eine Ausstrahlung auf andere Objekte im Inventar
             hat. Wird ausgeloest, wenn das Objekt ins Inventar hineinbewegt
             wird.
     damtype: eine Schadensart (DT_FIRE, DT_WATER, ...)
     treshold: hat zwei Bedeutungen abhaengig von dem Wert in list_key:
          1) Fuer Objekte mit SENSITIVE_INVENTORY oder SENSITIVE_ATTCK ist
             dies der Schadenswert, ab dem das Objekt benachrichtigt werden 
             soll.
             Hier wird ein Wert in "Defend-Einheiten" erwartet, d.h. das
             Zehnfache dessen, was am Ende in LP abgezogen wuerde.
          2) Fuer Objekte mit SENSITIVE_INVENTORY_TRIGGER ist dies der 
             Schadenswert, mit dem das Objekt andere bereits im Inventar
             des Containers befindliche Objekte beeinflusst, die eine 
             entsprechende Empfindlichkeit gesetzt haben
     options: Optionale Daten, die der programmierende Magier selbst
            definieren kann. Werden an die in den betroffenen Objekten
            aufgerufenen Funktionen durchgereicht.

     Ein SENSITIVE_ATTACK-Objekt, dessen Trigger-Bedingungen erfuellt sind,
     wird durch folgenden Funktionsaufruf darueber informiert:
       trigger_sensitive_attack(object enemy, string damtype, int damage,
                 mixed spell, mixed options)

     

     Ein SENSITIVE_INVENTORY-Objekt, dessen Trigger-Bedingungen erfuellt sind,
     wird durch folgenden Funktionsaufruf darueber informiert:
       trigger_sensitive_inv(object whodid, string damtype, int threshold,
                 mixed options, mixed options)

     Die beiden Funktionen muessen selbst ge-/ueberschrieben werden.

BEMERKUNGEN
-----------
::

     1. P_SENSITIVE-Objekte kosten Rechenzeit bei jedem Angriff oder jedem
        move() - daher bitte sparsam verwenden
     2. Ist P_SENSITIVE nicht statisch, sondern wird es situationsabhaengig 
        geaendert, muss man das environment() jeweils selbst ueber seine 
        neue Empfindlichkeit benachrichtigen. Dies geschieht mit den 
        Funktionen RemoveSensitiveObject() bzw.InsertSensitiveObject(), 
        siehe deren Manpages.

BEISPIEL
--------
::

     Beispiel 1:
     Beispielcode eines Objektes mit SENSITIVE_ATTACK und SENSITIVE_INVENTORY
     siehe hier: /doc/beispiele/testobjekte/attack_busy_sensitive_testobj.c

     Beispiel 2:
     Ein Eiszapfen, der bei Feuerangriffen oder bei heissen Gegenstaenden im
     gemeinsamen Environment zerschmelzen soll:

     void create() {
       SetProp( P_SENSITIVE, ({ ({SENSITIVE_ATTACK,     DT_FIRE, 100}),
                                 ({SENSITIVE_INVENTORY, DT_FIRE, 100}) }) );
       [...]
     }

     varargs void trigger_sensitive_attack() {
       remove();
     }

     varargs void trigger_sensitive_inv() {
       call_out("remove",0);  // verzoegert, wegen move()
     }

     varargs int remove(int silent) {
       if(!silent) {
         object room = all_environment(this_object())[<1];
         tell_room(room, Name()+" zerschmilzt.\n");
       }
       return ::remove();
     }

     - wird eine Fackel mit
       SetProp(P_SENSITIVE,({({SENSITIVE_INVENTORY_TRIGGER,DT_FIRE,250})}))
       in das gleiche environment() wie dieser Zapfen bewegt wird, loest
       diese im Zapfen trigger_sensitive_inv() aus
     - wird ein Angriff mit DT_FIRE und initialem Schaden > 100 auf das
       environment() veruebt, loest dies im Zapfen trigger_sensitive_attack()
       aus

SIEHE AUCH
----------
::

     Properties: P_SENSITIVE_ATTACK, P_SENSITIVE_INVENTORY,
                 P_SENSITIVE_INVENTORY_TRIGGER
     Funktionen: InsertSensitiveObject(L), RemoveSensitiveObject(L),
                 CheckSensitiveAttack(L), Defend(), 
                 insert_sensitive_inv(L), insert_sensitive_inv_trigger(L),
                 trigger_sensitive_inv(L), trigger_sensitive_attack(L)
     Defines:    /sys/sensitive.h

Letzte Aenderung: 10. Januar 2015, Arathorn

