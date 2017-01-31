.. Morgengrauen-Mudlib - sefun-Doku.

Ueber sefuns (simulated efuns)
==============================

sefuns sind Funktionen, die aehnlich wie echte efuns im Driver allen Objekten
(ausser dem Masterobjekt) zur Verfuegung stehen ohne dass die explizit geerbt
werden muessen. Sie sind allerdings in LPC implementiert und damit langsamer
als efuns.

Es ist moeglich, sefuns mit demselben Namen wie efuns zu erstellen, diese wird
dann anstelle der efun gerufen. Will man ausnahmsweise aber die efun mit dem
Namen rufen, so kann dies mit der Syntax ``efun::function()`` erreichen.

Verzeichnis der dokumentierten sefuns im Morgengrauen:

.. toctree::
   :maxdepth: 1
   :glob:

   sefun/*

