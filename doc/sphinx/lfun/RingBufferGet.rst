RingBufferGet()
===============

FUNKTION
--------
::

    protected mixed RingBufferGet(struct std_ringbuffer buf);

DEFINIERT IN
------------
::

    /std/util/ringbuffer.c
    /sys/util/ringbuffer.h

ARGUMENTE
---------
::

    buf - Ringpuffer, welcher ausgeben werden soll

BESCHREIBUNG
------------
::

    Diese Funktion liefert die Daten des Ringpuffers in Form eines Arrays
    zurueck, welches dann weiterverarbeitet werden kann.
    Die Reihenfolge der Daten ist entsprechend des beim Anlegen des
    Ringpuffers angebenen Modes:
    MODE_FIFO: aelteste Daten zuerst
    MODE_LIFO: neueste Daten zuerst

BEMERKUNGEN
-----------
::

    Aenderungen am Array beeinflussen die Daten des Ringpuffers nicht. Aber:
    Hierbei werden die Daten nicht tief kopiert. D.h. enthaelt der Ringpuffer
    Referenzen auf weitere Daten, zeigen der Ringpuffer und das hier
    gelieferte Array auf die gleichen Daten.

BEISPIELE
---------
::

    // Ringpuffer anlegen:
    struct std_ringbuffer buffer = CreateRingBuffer(10, MODE_FIFO);
    // 15 Werte reinschreiben:
    foreach(int i: 15) RingBufferPut(buffer, i);
    // Werte abrufen:
    mixed array=RingBufferGet(buffer);
    // array enthaelt nun:
    // ({5,6,7,8,9,10,11,12,13,14}) 

SIEHE AUCH
----------
::

    CreateRingBuffer(), RingBufferPut(), ResizeRingBuffer()

23.05.2008, Zesstra

