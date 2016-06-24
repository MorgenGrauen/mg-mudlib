#include <properties.h>
inherit "/std/room";

void create() {
  ::create();
  SetProp(P_LONG, "AddCmd-Testraum, Kommandos "
                  "\"kriech\" und \"schleiche&heran|herum\".");
  AddCmd("schleiche&heran|herum", "action_schleichen");
  AddExit("gilde", "/gilden/abenteurer");
}

void init() {
  ::init();
  add_action("action_kriechen", "kriech", 1);
}

static action_schleichen(string str) {
  string tmp = this_player()->QueryProp(P_RACE);
  if(tmp[<1]=='e') tmp=tmp[0..<2];
  write(break_string("Du versuchst leise zu schleichen, dabei passiert "
    "dir aber ein allzu "+
	(tmp=="Mensch"?"menschliches":lower_case(tmp)+"isches")+
	" Missgeschick. Verflucht!", 78));
  this_player()->command_me("\\furze");
  return 1;
}

static int action_kriechen(string str) {
  write(break_string("Deine Knie tun zu sehr weh dafuer.", 78));
  tell_room(this_object(), break_string(this_player()->Name(WER)+
    " knackt mit den Knien.", 78));
  return 1;
}