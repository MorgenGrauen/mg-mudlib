say
===

BEMERKUNGEN
-----------

  * da say() in der Vergangenheit wg. des Automatismus der Zielbestimmung oft
    an die falschen Spieler ausgegeben hat, wird von der Verwendung abgeraten.
  * Bitte ReceiveMsg() benutzen, sofern sinnvoll
  * fuer laengere Meldungen sollte break_string() verwendet werden
  * say(<str>) ist verhaltensgleich zu
    tell_room(environment(), <str>, ({this_player()||this_object()}))

