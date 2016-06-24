#pragma strict_types,save_types

inherit "/p/service/loco/std/mailcabin";

void create() {
  if (!clonep() || object_name(this_object()) == __FILE__[0..<3]) {
      set_next_reset(-1);
      return;
  }
  ::create();
}

