#pragma strong_types,rtt_checks

inherit "/std/channel_supervisor";

protected void create()
{
  ::create();
  // Namen setzen, der auf den Ebenen genannt werden soll.
  ch_set_sv_name("Wotan");
  // Das systemweite Init-File einlesen und die Zugriffsrechte fuer die Ebenen
  // merken, in denen dieses Objekt SV ist.
  ch_read_init_file();
  // Alternativ kann ein eigenes File im gleichen Format genutzt werden. (ggf.
  // ist dann eine eUID noetig!)
  // ch_read_init_file("/path/to/custom/initfile");
}

