// Beispielraum fuer P_NETDEAD_INFO-Funktionalitaet
#include <properties.h>
#include <break_string.h>
inherit "/std/room";
     
string create_destiny(mixed val);
int create_room(string dir);

void create() {
  ::create();

  if (clonep(this_object())) {
    // setze Informationen, die in einem Netztoten gespeichert werden sollen
    Set(P_NETDEAD_INFO, random(5));
    SetProp(P_INT_LONG, break_string(
      "Wenn du hier einschlaefst, wird der Raum nach 30s zerstoert. Beim "
      "Aufwachen wirst du an die Blueprint dieses Raums die Info "+
      QueryProp(P_NETDEAD_INFO)+" uebergeben. Diese wird aus dieser Info "
      "einen Raumpfad ermitteln, in den du bewegt wirst.", 78));
  } else {
    // Blueprint: hier kann man zu einem Cloneraum gehen
    AddExit("cloneraum", #'create_room);
    SetProp(P_INT_LONG, break_string(
      "Zum Testen einfach den Ausgang 'cloneraum' benutzen. Es wird dann "
      "ein Raum geclont, in den man bewegt wird. Wenn man dort einschlaeft, "
      "wird der geclonte Raum nach circa 30s zerstoert.\n"
      "Beim Aufwachen werden die im Spieler gespeicherten Informationen des "
      "geclonten (und nunmehr zerstoerten) Raumes benutzt, um einen "
      "alternativen Aufwachraum (Klerusgilde, Karategilde, Wald oder Port "
      "Vain) zu bestimmen.\n"
      "Andernfalls wuerde der Spieler in der Abenteurergilde erwachen.", 78,
      0, BS_LEAVE_MY_LFS));
  }

  // Set-Method, um die Informationen aus P_NETDEAD_INFO beim Aufwachen
  // in der Blueprint auswerten zu koennen
  Set(P_NETDEAD_INFO, #'create_destiny, F_SET_METHOD);
  SetProp(P_LIGHT, 1);
}
     
// Raum entfernen, normalerweise so KEINE GUTE IDEE!
void BecomesNetDead(object pl) {
  call_out(#'remove, 30);
}

// erzeuge einen Cloneraum und bewege den Spieler dahin
int create_room(string dir) {
  object dest = clone_object(object_name(this_object()));
  this_player()->move(dest, M_NOCHECK);
  return 1;
}
     
// Set-Method fuer P_NETDEAD_INFO: gibt Pfad zurueck
// benutze die Informationen aus dem jetzt aufwachenden Netztoten, um einen
// alternativen Aufwachraum zu ermitteln, da der Einschlafraum zerstoert ist
string create_destiny(mixed val) {
  if (intp(val)) {
    switch (val) {
      case 0:
        return "/d/ebene/room/PortVain/po_haf1";
      case 1:
        return "/gilden/klerus";
      case 2:
        return "/gilden/karate";
      default:
    }
    return "/d/ebene/room/waldweg4";
  }
}
