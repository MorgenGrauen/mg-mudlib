P_COMPILER_PATH
===============

NAME
----
::

    P_COMPILER_PATH               "compiler_path"               

DEFINIERT IN
------------
::

    /sys/v_compiler.h

BESCHREIBUNG
------------
::

    Directory in dem ein Virtual Compiler Objekte erzeugt.
    Muss im virtual_compiler.c gesetzt werden.

    Der VirtualCompiler muss nicht zwingend in diesem Verzeichnis
    liegen, um zu funktionieren, sollte es aber, um die Zuordnung des VCs zu
    "seinen" Objekten zu erleichern.

BEISPIEL
--------
::

    SetProp(P_COMPILER_PATH,"/d/region/magier/vc/");

SIEHE AUCH
----------
::

    P_STD_OBJECT, virtual_compiler

Letzte Aenderung: 23.10.2007, von Zesstra

