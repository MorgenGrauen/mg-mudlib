QueryBuyValue()
===============

QueryBuyValue()

Funktion
    static varargs int QueryBuyValue(mixed ob, object client)

Definiert in
    /std/room/shop

Argumente
    ob 
      Das zu kaufende Objekt (String oder object).
      Im Normalfall handelt es sich um ein Objekt. Ausnahme sind 
      Gegenstaende, die mit AddFixedObject() hinzugefuegt wurden.
    client 
      Der Kaeufer.

Beschreibung
    Ermittelt den Preis, den <client> fuer <ob> zu bezahlen hat.

Rueckgabewert
    Der Preis als Integer.

Beispiel
    Ein Haendler, der Spielern die ihm geholfen haben einen Rabatt von 10% 
    gewaehrt
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
::

    

    object *helpers;
    protected void create()
    {
      ::create();
      helpers=({});
      ...
    }

    

    static varargs int QueryBuyValue(mixed ob, object client)
    {
      if(member(helpers,client)!=-1)
      {
        return ::QueryBuyValue(ob,client)*9/10;
      }
      return ::QueryBuyValue(ob,client);
    }

Siehe auch:
    Funktionen:
      AddFixedObject(), RemoveFixedObject(), SetStorageRoom(), 
      QueryStorageRoom(), QueryBuyFact(), sell_obj(), buy_obj()
    Properties:
      P_KEEPER, P_MIN_STOCK, P_STORE_CONSUME


Letzte Aenderung: 21.05.2014, Bugfix

