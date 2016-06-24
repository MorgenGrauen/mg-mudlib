#include <defines.h>
#include <moving.h>
#include <properties.h>
#include <language.h>

#define SPIELER "nefis"
#define DEBUG(x)  if (find_player("zesstra"))\
            tell_object(find_player("zesstra"),\
		                      "Nefis P_SIZE: "+x+"\n")
#define LOG(x) log_file("zesstra/P_SIZE.log", x)

private nosave object pl;

void create()
{
  if (!clonep() ) return;
  pl = find_player(SPIELER) || find_netdead(SPIELER);
  if (!objectp(pl)) {
      DEBUG("Playerobjekt nicht gefunden.");
      destruct(ME);
      return;
  }
  shadow( pl, 1);
  DEBUG(sprintf("%O shadowed by: %O\n", pl, shadow(pl, 0)));
}

int _set_size(int sz) {
  LOG(sprintf("[%s] Aenderung von P_SIZE in %O auf %O,\nCallerstack: %O\n\n",
	strftime("%c",time()), pl, sz, caller_stack()));
  return pl->Set(P_SIZE, sz, F_VALUE);
}

public varargs mixed Set( string name, mixed Value, int Type, int extern ) {
  if (name == P_SIZE) {
  LOG(sprintf("[%s] Aenderung von P_SIZE in %O auf %O (Typ: %O) [Set()!]"
	",\nCallerstack: %O\n\n",
	        strftime("%c",time()), pl, Value, Type, caller_stack()));
  }
  return pl->Set(name, Value, Type, extern);
}

int stop() {
  unshadow();
  destruct(ME);
  return 1;
}

void reset() {
  if (!objectp(pl)) 
    stop();
}

int remove() {
  stop();
  return 1;
}

