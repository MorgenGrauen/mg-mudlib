// MorgenGrauen MUDlib
//
// living/moneyhandler.c -- money handler for livings
//
// $Id: moneyhandler.c 6738 2008-02-19 18:46:14Z Humni $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/container/moneyhandler";

// Funktionen sollen nur das Programm ersetzen, natuerlich nur in der
// Blueprint _dieses_ Objektes, nicht in anderen. ;-) BTW: Normalerweise
// sollte niemand hierdrin create() rufen, der das Ding hier erbt.
protected void create_super() {
  if (object_name(this_object()) == __FILE__[..<3])
    replace_program();
}

// wird nicht von erbenden Objekten gerufen. (Wozu auch.)
protected void create() {
  create_super();
}

