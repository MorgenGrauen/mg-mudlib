CreateRingBuffer()
==================

FUNKTION
--------
::

    protected struct std_ringbuffer CreateRingBuffer(int size, int newmode);

DEFINIERT IN
------------
::

    /std/util/ringbuffer.c
    /sys/util/ringbuffer.h

ARGUMENTE
---------
::

    size    - Groesse des neuen Ringpuffers (int)
    newmode - Ausgabemodus beim Abrufen des Puffers (int):
              MODE_FIFO: First-in-First-Out
              MODE_LIFO: Last-in-First-Out

BESCHREIBUNG
------------
::

    Diese Funktion erstellt einen neuen, leeren Ringpuffer der Groesse <size>
    und liefert ihn zurueck. Die Daten des Puffers werden spaeter gemaess
    <newmode> so gespeichert, dass bei der Ausgabe des Puffers mittels
    RingBufferGet() die entweder die neuesten Daten zuerst (MODE_LIFO) oder
    die aeltesten Daten zuerst (MODE_FIFO) geliefert werden.

RUeCKGABEWERT
-------------
::

    Der neue Ringpuffer. Dieser wird in einer Struct std_ringbuffer
    gespeichert. Er ist in einer Variable 'mixed' oder in einer mittels
    'struct std_ringbuffer' angelegten Variable speicherbar.

BEMERKUNGEN
-----------
::

    Der gelieferte Ringpuffer sollte nicht per Hand verarbeitet oder
    genaendert werden, sondern nur ueber die Verwaltungsfunktionen aus
    /std/util/ringbuffer.c.

BEISPIELE
---------
::

    // Variable anlegen:
    struct std_ringbuffer buffer;
    // _oder_: mixed buffer;
    // neuen Puffer mit max. 50 Elementen anlegen, der bei der Abfrage die
    // aeltesten Daten zuerst zurueckliefert:
    buffer = CreateRingBuffer(50, MODE_FIFO);

SIEHE AUCH
----------
::

    RingBufferPut(), RingBufferGet(), ResizeRingBuffer()

23.05.2008, Zesstra

