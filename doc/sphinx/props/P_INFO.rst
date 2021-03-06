P_INFO
======

NAME
----
::

     P_INFO                        "info"                        

DEFINIERT IN
------------
::

     /sys/properties.h

BESCHREIBUNG
------------
::

     Geheime Information, die ggf. ueber einen Zauberspruch
     von Spielern ermittelt werden kann.

     

BEISPIEL
--------

.. code-block:: pike

     // Einfaches Beispiel
     SetProp(P_INFO, "Du ergruendest das Geheimnis.\n")

.. code-block:: pike

     // dynamisch
     protected string my_info();

     void create() {
       if(clonep(this_object())) return;
       ::create();

       Set(P_INFO, #'my_info, F_QUERY_METHOD);
       // [...]
     }

     protected string my_info() {
       return(break_string(
         "Die "+cnt+" Steine tragen ein geheimes Muster in sich.", 78));
     }


SIEHE AUCH
----------

    Aehnlich:
      :doc:`../lfun/GetOwner`

14. Mar 2017 Gloinson
