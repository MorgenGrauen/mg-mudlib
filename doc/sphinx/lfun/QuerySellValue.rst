QuerySellValue()
================

FUNKTION
--------
::

      varargs int QuerySellValue(object ob, object client);

DEFINIERT IN
------------
::

      /std/trading_price.c

BESCHREIBUNG
------------
::

      Diese Funktion kann dazu genutzt werden, den Verkaufspreis in einem 
      Laden global zu veraendern. Sofern dies zugunsten der Spieler geschieht,
      muss dafuer die Zustimmung des zustaendigen Regionsmagiers und der
      Balance eingeholt werden. Zudem sollte es nicht in jedem x-beliebigen 
      Laden genutzt werden, sondern nur in recht abgelegenen Gebieten, in
      die man nicht einfach skripten kann.

      Ein Beispiel ist der Laden auf dem Kutter in Port Vain, der nur in
      laengeren Intervallen mal zugaenglich ist.

BEISPIEL
--------
::

      Ein Laden zahlt 10% ueber dem normalen Verkaufspreis:

      private int sell_factor = 110;

      varargs int QuerySellValue(object ob, object client) {
        // Es wird nicht naeher geprueft, ob <ob> ein gueltiger Objektpointer
        // ist, da der Laden selbst sicherstellt, dass das so ist. Wenn 
        // das doch einmal nicht der Fall sein sollte, liegt der Fehler
        // woanders. Einen Bug auszuloesen ist dann sinnvoll.

        // Basispreis ermitteln 
        int preis = ::QuerySellValue(ob, client);
        // und den Bonus aufschlagen.
        preis = (sell_factor * preis)/100; 

        // Nicht mehr auszahlen, als das Objekt an sich wert ist.
        return min(preis, ob->QueryProp(P_VALUE));
      }


LETZTE AENDERUNG: 18-Jun-2015, Arathorn

