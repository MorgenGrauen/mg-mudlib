SetTimedAttrModifier()
======================

FUNKTION
--------
::

     int SetTimedAttrModifier(string key, mapping modifier, 
                              int outdated, object dependent, mixed notify) 

DEFINIERT IN
------------
::

     /std/living/attributes.c

ARGUMENTE
---------
::

     key	-	in P_TIMED_ATTR_MOD vorzunehmender oder zu 
                        aendernder Eintrag
     modifier   -       Mapping mit den Attributveraenderungen
     outdated   -       Zeitpunkt zu dem die Attributveraenderungen
                        ablaufen sollen in Sekunden seit dem
                        1. Jan 1970, 0.0:0 GMT oder 0
     dependent  -       Objekt dessen Existenz eine Bedingung fuer die
                        Attributveraenderung sein soll oder 0
     notify     -       Objekt oder File das mittels 
                        NotifyTimedAttrModExpired ueber
                        den Attributablauf informiert werden soll

BESCHREIBUNG
------------
::

     Der zu key gehoerende Eintrag wird in P_TIMED_ATTR_MOD angelegt oder
     modifiziert und update_max_sp_and_hp aufgerufen.
     Es empfiehlt sich auf die Eindeutigkeit des string-Parameters key
     besonderes Augenmerk zu legen.

     Unter dem Key key wird in P_TIMED_ATTR_MOD ein Eintrag vorgenommen,
     welcher die Attribute des Livings um die in modifier stehenden Offsets
     veraendert.

     Diese Veraenderung ist solange aktiv bis entweder die in outdated
     stehende Zeit ueberschritten ist oder das in dependent uebergebene
     Objekt nicht mehr existiert.
     Sind beide Argumente 0 so laeuft die Attributveraenderung nicht ab
     und kann durch DeleteTimedAttrModifier geloescht werden.
     Laeuft die Attributveraenderung ab, so wird der in notify angegebene
     Empfaenger mittels Aufruf NotifyTimedAttrModExpired davon 
     benachrichtigt.
     Der Funktion NotifyTimedAttrModExpired wird als Argument der key
     der abgelaufenen Attributveraenderung uebergeben.

BEISPIELE
---------
::

     Ein NPC kann einen Spieler auf die folgende Weise solange die
     Attribute um eins herabsetzen bis entweder eine Stunde verstrichen
     ist oder der NPC nicht mehr existiert zum Beispiel weil er getoetet
     wurde.

       player->SetTimedAttrModifier( player->query_real_name(),
                                     ([A_STR:-1,A_INT:-1,A_DEX:-1,A_CON:-1]),
                                     time()+3600,
                                     this_object(),
                                     this_object()
                                   );

     Will der NPC nun noch darauf reagieren, dass die Attributmodifikation
     durch Timeout abgelaufen ist, so koennte dies folgendermassen geschehen.

       void NotifyTimedAttrModExpired(string str)
       {
           // Die Funktion wird aus dem Lebewesen gerufen, in welchem der Mod
           // gerade abgelaufen ist. Also Meldungsausgabe an
           // previous_object().
           tell_object(previous_object()
               ,"Da hast Du aber nochmal Glueck gehabt.\n");
       }

RUeCKGABEWERT
-------------
::

     TATTR_INVALID_ARGS      -     Im Falle eines fehlenden key-Arguments,
                                   eines fehlenden modifier-Arguments oder
                                   eines bereits abgelaufenen
                                   outdatet-Arguments
     TATTR_OK                -     Im Erfolgsfall

     Die Rueckgabewerte sind in /sys/living/attributes.h definiert.

SIEHE AUCH
----------
::

     Attribute:  QueryAttribute(), SetAttribute()
                 SetRealAttribute(), QueryRealAttribute(),
                 QueryAttributeOffset(),
                 UpdateAttributes()
     Methoden:   QueryTimedAttrModifier(), DeleteTimedAttrModifier()
     Callback:   NotifyTimedAttrModExpired()
     Properties: P_ATTRIBUTES, P_ATTRIBUTES_OFFSETS
		 P_X_ATTR_MOD, P_M_ATTR_MOD
		 P_TIMED_ATTR_MOD
     Sonstiges:  /std/living/attributes.c

LETZTE Aenderung:
15.02.2009, Zesstra

