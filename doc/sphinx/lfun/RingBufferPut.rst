RingBufferPut()
===============

FUNKTION
--------
::

    protected void RingBufferPut(struct std_ringbuffer buf, mixed val);

DEFINIERT IN
------------
::

    /std/util/ringbuffer.c
    /sys/util/ringbuffer.h

ARGUMENTE
---------
::

    buf - Ringpuffer, in den <val> geschrieben werden soll
    val - neues Datum

BESCHREIBUNG
------------
::

    Diese Funktion schreibt <val> in den Ringpuffer <buf>. Hierbei wird ggf.
    das aelteste im Puffer existierende Datum ueberschrieben. <buf> muss eine
    von CreateRingBuffer() oder ResizeRingBuffer() zurueckgelieferter
    Ringpuffer sein.

BEISPIELE
---------
::

    // Ringpuffer anlegen:
    struct std_ringbuffer buffer = CreateRingBuffer(10, MODE_FIFO);
    // 15 Werte reinschreiben:
    foreach(int i: 15) RingBufferPut(buffer, i);

SIEHE AUCH
----------
::

    RingBufferGet(), CreateRingBuffer(), ResizeRingBuffer()

23.05.2008, Zesstra

