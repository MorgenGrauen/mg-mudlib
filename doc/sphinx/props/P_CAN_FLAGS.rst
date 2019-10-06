P_CAN_FLAGS
===========

NAME
----
::

    P_CAN_FLAGS                   "can_flags"                   

DEFINIERT IN
------------
::

    /sys/player/can.h

BESCHREIBUNG
------------
::

    Flags, die bestimmte Befehle oder Funktionen freischalten.

    emote-Kommando:      CAN_EMOTE
    echo-Kommando:       CAN_ECHO
    r-emote-Kommando:    CAN_REMOTE
    Ausgabe des Presays: CAN_PRESAY
    Reportfunktionen:    KP - CAN_REPORT_SP 
                         Gift - CAN_REPORT_POISON 
                         Vorsicht - CAN_REPORT_WIMPY 
                         Fluchtrichtung - CAN_REPORT_WIMPY_DIR

    Zum Setzen muessen die Flags ver-oder-t werden:
    
    SetProp(P_CAN_FLAGS, CAN_EMOTE|CAN_ECHO|CAN_REPORT_SP);

Letzte Aenderung: 2019-Sep-15, Arathorn
