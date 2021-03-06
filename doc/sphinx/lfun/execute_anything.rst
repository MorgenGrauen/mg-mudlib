execute_anything()
==================

FUNKTION
--------
::

    protected mixed execute_anything(mixed fun, mixed args, ...)

DEFINIERT IN
------------
::

    /std/util/executer.c

ARGUMENTE
---------
::

    mixed fun        auszufuehrende Funktion/Array/Closure ... anything
    mixed args       weitere Argumente, werden weiter uebergeben

BESCHREIBUNG
------------
::

    Fuehrt "alles" aus:

    

    'fun' kann sein:
    - eine Closure
      [funcall(fun, args, ...)]
    - einen String als Funktion im Objekt
      [call_other(this_object(), fun, args, ...)]
    - ein Array mit Objekt/Objektnamen + Funktion als Aufruf der Funktion am
      Objekt/Objektnamen:
      [call_other(array[0], array[1], args...)]

    Wird execute_anything() aus dem restriction_checker gerufen, wird als
    erster Parameter immer das gepruefte Living uebergeben.

BEMERKUNGEN
-----------
::

    Es kann sich anbieten, die gerufene Funktion mit varargs-Argumenten zu
    gestalten, damit ihr mehr Argumente uebergeben werden koennen als in der
    Funktionsdefinition angegeben sind. 

    Der Restriktionspruefer (/std/restriction_checker) versteht im Falle eines
    Arrays mit Objekt/Objektnamen + Funktion auch ein Array mit mehr als 2
    Elementen. Die weiteren Elemente werden dann zu weiteren Argumenten der
    gerufenen Funktion. Hierbei wird <pl> ggf. aber als erstes Argument vor
    alle anderen eingefuegt.

SIEHE AUCH
----------
::

    check_restrictions, varargs

31.12.2013, Zesstra

