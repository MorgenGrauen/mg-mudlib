// MorgenGrauen MUDlib
//
// player/vitems.c -- no vitems for players
//
#pragma strict_types,rtt_checks, range_check
#pragma no_clone

protected object *GetVItemClones()
{
  return ({});
}

protected object present_vitem(string complex_desc)
{
  return 0;
}
