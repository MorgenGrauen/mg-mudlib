#pragma strong_types,rtt_checks,no_clone,no_inherit,no_shadow

CURRENT_VERSION 2

// Liefert die URL zum Download und die aktuelle Version des
// MorgenGrauen.mpackage fuer Mudlet.
public <int|string>* current_version()
{
  return ({"https://mg.mud.de/download/MorgenGrauen.mpackage",
           CURRENT_VERSION});
}

