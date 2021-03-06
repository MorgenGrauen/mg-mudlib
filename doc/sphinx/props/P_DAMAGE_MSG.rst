P_DAMAGE_MSG
============

NAME
----
::

     P_DAMAGE_MSG      "std_p_dam_msg"

DEFINIERT IN
------------
::

     /sys/living/combat.h

BESCHREIBUNG
------------
::

     In dieser Property lassen sich individuelle Treffer-/Schadensmeldungen
     fuer dieses Lebewesen festlegen. Sie werden verwendet, falls bei
     eingehendem Schaden der Aufrufer von Defend() Schadensmeldungen wuenscht
     (d.h. SP_SHOW_DAMAGE != 0), jedoch keine eigenen Meldungen vorgibt.

     Enthaelt diese Property kein Array, werden ggf. die Standardmeldungen
     ausgegeben.

     Datenstruktur der Property:
       ({
        ({ int lphit1, string mess_me,
                       string mess_en,
                       string mess_room }),
        ({ lphit2, mess_me, mess_en, mess_room }),
        ...
        ({ lphitn, mess_me, mess_en, mess_room }),
       })
       wobei lphit1<lphit2<...<lphitn sein muss, d.h. das Array-
       Array ist aufsteigend sortiert.

     Ist ein Treffer x LP hart, werden die Meldungen des lphit-
     Arrays ausgegeben, dessen Wert am naechsten unter dem Schaden
     liegt.

     In den Meldungen mess_me (an den Getroffenen), mess_en (an
     den Feind), mess_room (an die restlichen Umstehenden) koennen
     Ersatzstrings wie folgt verwendet werden:
     @WER1/@WESSEN1/@WEM1/@WEN1 - name(casus) des Getroffenen (TO)
     @WER2/@WESSEN2/@WEM2/@WEN2 - name(casus) des Feindes (enemy)

SIEHE AUCH
----------
::

     Defend()
     /std/living/combat.c

15.09.2010, Zesstra

