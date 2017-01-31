P_VISIBLE_GUILD
===============

NAME
----
::

     P_VISIBLE_GUILD			"visible_guild"                       

DEFINIERT IN
------------
::

     /sys/properties.h

BESCHREIBUNG
------------
::

     Diese Property enthaelt die sichtbare Gilde des Lebewesens in Form eines
     kleingeschriebenen Strings, also die Gilde, die bei Spielern zum
     Beispiel bei 'kwer' oder 'finger' angezeigt wird. So kann man fremde
     Gilden testen und trotzdem nach aussen hin in der gleichen Gilde wie
     zuvor bleiben.

BEISPIEL
--------
::

     Wenn man gerne nach aussen hin Zauberer bleiben moechte:
	  pl->SetProp(P_VISIBLE_GUILD,"zauberer");
     Nach aussen hin bleibt man jetzt auch Zauberer, wenn P_GUILD eine
     andere Gilde angibt.

SIEHE AUCH
----------
::

     P_GUILD, P_DEFAULT_GUILD


Last modified: Wed Jan 14 19:17:06 1998 by Patryn

