// MorgenGrauen MUDlib
//
// armour/moving.c -- armour moving object
//
// $Id: moving.c 6306 2007-05-20 11:32:03Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

// Dies muss eigentlich nix ueber clothing/moving hinausgehendes koennen.
// Die Ruestungen erben auch alle direkt clothing/moving, dieses File
// existiert eigentlich nur fuer den Fall, dass Leute ausserhalb von /std von
// armour/moving erben.
inherit "/std/clothing/moving";

