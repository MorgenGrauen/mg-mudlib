/*
  1)
    Das Package (*.mpackage) dort ablegen, wo
      <website>/download/MorgenGrauen.mpackage
    hinzeigt. (lesbar fuer alle, d.h. Zugriffsrechte 644 oder so)

  2)
    In /std/player/protocols/mudlet_gui.c den Versionscounter
    (CURRENT_VERSION) inkrementieren und das File neuladen.
*/

#pragma strong_types,rtt_checks,no_clone,no_inherit,no_shadow

#define CURRENT_VERSION 3

// Liefert die URL zum Download und die aktuelle Version des
// MorgenGrauen.mpackage fuer Mudlet.
public <int|string>* current_version()
{
  return ({"https://mg.mud.de/download/MorgenGrauen.mpackage",
           CURRENT_VERSION});
}

